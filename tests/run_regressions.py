#!/usr/bin/env python3
import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parent
EXPECTED_RE = re.compile(r"^\| \x60([^\x60]+\.tc)\x60 \|\s*(\d+)\s*\|")
I32_MIN = -(1 << 31)


def i32(value):
    value &= 0xFFFFFFFF
    return value - (1 << 32) if value & 0x80000000 else value


def u32(value):
    return value & 0xFFFFFFFF


def parse_expected():
    cases = []
    for line in (ROOT / "README.md").read_text(encoding="utf-8").splitlines():
        match = EXPECTED_RE.match(line)
        if match:
            cases.append((ROOT / match.group(1), int(match.group(2))))
    return cases


class Machine:
    def __init__(self, assembly):
        self.instructions = []
        self.labels = {}
        self.data_labels = {}
        self.memory = {}
        self.registers = {"sp": 0x100000, "ra": -1}
        self.executed_instructions = 0
        self.call_count = 0
        self.op_counts = {}
        self._parse(assembly)

    def _parse(self, assembly):
        section = None
        data_address = 0x10000
        pending_data_label = None
        for raw in assembly.splitlines():
            line = raw.split("#", 1)[0].strip()
            if not line:
                continue
            if line == ".data":
                section = "data"
                continue
            if line == ".text":
                section = "text"
                continue
            if line.endswith(":"):
                label = line[:-1]
                if section == "data":
                    pending_data_label = label
                    self.data_labels[label] = data_address
                else:
                    self.labels[label] = len(self.instructions)
                continue
            if section == "data" and line.startswith(".word"):
                value = int(line.split(None, 1)[1], 0)
                self.memory[data_address] = i32(value)
                data_address += 4
                pending_data_label = None
                continue
            if line.startswith("."):
                continue
            if section == "text":
                self.instructions.append(line)
        if pending_data_label is not None:
            self.data_labels[pending_data_label] = data_address

    def get(self, name):
        return 0 if name == "zero" else self.registers.get(name, 0)

    def set(self, name, value):
        if name != "zero":
            self.registers[name] = i32(value)

    @staticmethod
    def split(line):
        return [part for part in re.split(r"[\s,]+", line.strip()) if part]

    @staticmethod
    def address(operand):
        match = re.fullmatch(r"(-?\d+)\((\w+)\)", operand)
        if not match:
            raise AssertionError(f"invalid address: {operand}")
        return int(match.group(1)), match.group(2)

    @staticmethod
    def quotient(lhs, rhs):
        if rhs == 0:
            return -1
        if lhs == I32_MIN and rhs == -1:
            return I32_MIN
        value = abs(lhs) // abs(rhs)
        return -value if (lhs < 0) != (rhs < 0) else value

    def run(self, max_steps=10_000_000):
        if "main" not in self.labels:
            raise AssertionError("assembly has no main label")
        pc = self.labels["main"]
        for _ in range(max_steps):
            line = self.instructions[pc]
            parts = self.split(line)
            op, args = parts[0], parts[1:]
            self.executed_instructions += 1
            self.op_counts[op] = self.op_counts.get(op, 0) + 1
            if op == "call":
                self.call_count += 1
            next_pc = pc + 1

            if op == "li":
                self.set(args[0], int(args[1], 0))
            elif op == "la":
                self.set(args[0], self.data_labels[args[1]])
            elif op == "addi":
                self.set(args[0], self.get(args[1]) + int(args[2], 0))
            elif op == "add":
                self.set(args[0], self.get(args[1]) + self.get(args[2]))
            elif op == "sub":
                self.set(args[0], self.get(args[1]) - self.get(args[2]))
            elif op == "mul":
                self.set(args[0], self.get(args[1]) * self.get(args[2]))
            elif op == "div":
                self.set(args[0], self.quotient(self.get(args[1]), self.get(args[2])))
            elif op == "rem":
                lhs, rhs = self.get(args[1]), self.get(args[2])
                quotient = self.quotient(lhs, rhs)
                self.set(args[0], lhs if rhs == 0 else lhs - quotient * rhs)
            elif op == "slli":
                self.set(args[0], u32(self.get(args[1])) << (int(args[2], 0) & 31))
            elif op == "slt":
                self.set(args[0], int(self.get(args[1]) < self.get(args[2])))
            elif op == "sltu":
                self.set(args[0], int(u32(self.get(args[1])) < u32(self.get(args[2]))))
            elif op == "slti":
                self.set(args[0], int(self.get(args[1]) < int(args[2], 0)))
            elif op == "sltiu":
                self.set(args[0], int(u32(self.get(args[1])) < u32(int(args[2], 0))))
            elif op == "xori":
                self.set(args[0], u32(self.get(args[1])) ^ u32(int(args[2], 0)))
            elif op in {"lw", "sw"}:
                offset, base = self.address(args[1])
                address = u32(self.get(base) + offset)
                if op == "lw":
                    self.set(args[0], self.memory.get(address, 0))
                else:
                    self.memory[address] = self.get(args[0])
            elif op == "jal":
                self.set(args[0], pc + 1)
                next_pc = self.labels[args[1]]
            elif op == "call":
                self.set("ra", pc + 1)
                next_pc = self.labels[args[0]]
            elif op == "ret":
                next_pc = self.get("ra")
                if next_pc == -1:
                    return u32(self.get("a0")) & 0xFF
            elif op in {"beq", "bne", "blt", "bge"}:
                lhs, rhs = self.get(args[0]), self.get(args[1])
                taken = {
                    "beq": lhs == rhs,
                    "bne": lhs != rhs,
                    "blt": lhs < rhs,
                    "bge": lhs >= rhs,
                }[op]
                if taken:
                    next_pc = self.labels[args[2]]
            else:
                raise AssertionError(f"unsupported instruction: {line}")

            pc = next_pc
        raise AssertionError(f"program exceeded {max_steps} simulated instructions")


def compile_case(compiler, source, extra_args):
    command = [str(compiler), *extra_args]
    result = subprocess.run(
        command,
        input=source.read_text(encoding="utf-8"),
        text=True,
        capture_output=True,
        check=False,
    )
    if result.returncode != 0:
        raise AssertionError(f"{source.name}: compiler failed: {result.stderr.strip()}")
    return result.stdout


def main():
    compiler = pathlib.Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT.parent / "build" / "toyc.exe"
    failures = []
    cases = parse_expected()
    modes = [
        ("normal", []),
        ("-opt", ["-opt"]),
        ("-opt backend", ["-opt", "-fno-ctfe"]),
    ]
    for source, expected in cases:
        for mode, extra_args in modes:
            try:
                assembly = compile_case(compiler, source, extra_args)
                machine = Machine(assembly)
                if mode == "-opt" and len(machine.instructions) != 2:
                    failures.append(
                        f"{source.relative_to(ROOT)} [{mode}]: CTFE emitted {len(machine.instructions)} instructions"
                    )
                actual = machine.run()
                if actual != expected:
                    failures.append(f"{source.relative_to(ROOT)} [{mode}]: expected {expected}, got {actual}")
            except Exception as error:
                failures.append(f"{source.relative_to(ROOT)} [{mode}]: {error}")
    if failures:
        print("\n".join(failures), file=sys.stderr)
        return 1
    print(f"passed {len(cases)} cases in normal, -opt, and -opt backend modes")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
