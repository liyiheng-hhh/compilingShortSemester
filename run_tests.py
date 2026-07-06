#!/usr/bin/env python3

import argparse
import os
import subprocess
import sys
import re
from pathlib import Path


def parse_expected(expected_path):
    if not expected_path.exists():
        return {"exit_code": None, "stdout_match": None, "stderr_match": None}

    with open(expected_path, "r", encoding="utf-8") as f:
        content = f.read()

    result = {"exit_code": None, "stdout_match": None, "stderr_match": None}

    for line in content.splitlines():
        line = line.strip()
        if line.startswith("EXIT_CODE:"):
            parts = line.split(":", 1)
            if len(parts) == 2:
                result["exit_code"] = int(parts[1].strip())
        elif line.startswith("STDOUT_MATCH:"):
            parts = line.split(":", 1)
            if len(parts) == 2:
                result["stdout_match"] = parts[1].strip()
        elif line.startswith("STDERR_MATCH:"):
            parts = line.split(":", 1)
            if len(parts) == 2:
                result["stderr_match"] = parts[1].strip()

    return result


def build_compiler(build_dir, opt=False):
    os.makedirs(build_dir, exist_ok=True)

    cmake_cmd = ["cmake", "..", f"-DCMAKE_BUILD_TYPE={'Release' if opt else 'Debug'}"]

    try:
        subprocess.run(
            cmake_cmd,
            cwd=build_dir,
            check=True,
            capture_output=True,
            text=True,
        )
    except subprocess.CalledProcessError as e:
        print(f"CMake failed:\n{e.stderr}")
        return None

    build_cmd = ["cmake", "--build", "."]

    try:
        subprocess.run(
            build_cmd,
            cwd=build_dir,
            check=True,
            capture_output=True,
            text=True,
        )
    except subprocess.CalledProcessError as e:
        print(f"Build failed:\n{e.stderr}")
        return None

    compiler_path = os.path.join(build_dir, "toyc.exe" if sys.platform == "win32" else "toyc")
    if not os.path.exists(compiler_path):
        print(f"Compiler not found: {compiler_path}")
        return None

    return compiler_path


def run_compiler(compiler_path, source, opt=False):
    args = [compiler_path]
    if opt:
        args.append("-opt")

    result = subprocess.run(
        args,
        input=source,
        capture_output=True,
        text=True,
    )

    return {
        "stdout": result.stdout,
        "stderr": result.stderr,
        "returncode": result.returncode,
    }


def run_assembly(asm_code, test_name, temp_dir):
    asm_file = os.path.join(temp_dir, f"{test_name}.s")
    with open(asm_file, "w", encoding="utf-8") as f:
        f.write(asm_code)

    qemu_riscv32 = "qemu-riscv32"
    spike = "spike"

    for emulator in [qemu_riscv32, spike]:
        try:
            result = subprocess.run(
                ["which" if sys.platform != "win32" else "where", emulator],
                capture_output=True,
                text=True,
            )
            if result.returncode == 0:
                break
        except FileNotFoundError:
            continue
    else:
        return {"emulator": None, "exit_code": None, "stdout": None, "stderr": None}

    if emulator == "qemu-riscv32":
        result = subprocess.run(
            [emulator, asm_file],
            capture_output=True,
            text=True,
        )
        return {
            "emulator": "qemu-riscv32",
            "exit_code": result.returncode,
            "stdout": result.stdout,
            "stderr": result.stderr,
        }
    elif emulator == "spike":
        result = subprocess.run(
            [spike, "-d", asm_file],
            capture_output=True,
            text=True,
        )
        exit_code = None
        for line in result.stderr.splitlines():
            if "tohost" in line.lower():
                match = re.search(r"0x([0-9a-f]+)", line, re.IGNORECASE)
                if match:
                    exit_code = int(match.group(1), 16)
                    break

        return {
            "emulator": "spike",
            "exit_code": exit_code,
            "stdout": result.stdout,
            "stderr": result.stderr,
        }


def run_test(compiler_path, tc_file, temp_dir, opt=False):
    test_name = os.path.basename(tc_file).replace(".tc", "")
    expected_path = Path(str(tc_file).replace(".tc", ".expected"))
    expected = parse_expected(expected_path)

    with open(tc_file, "r", encoding="utf-8") as f:
        source = f.read()

    compile_result = run_compiler(compiler_path, source, opt)

    passed = True
    messages = []

    if compile_result["returncode"] != 0:
        messages.append(f"Compiler failed with exit code {compile_result['returncode']}")
        if expected["exit_code"] is not None and expected["exit_code"] != 0:
            passed = True
        else:
            passed = False
        if compile_result["stderr"]:
            messages.append(f"Compiler stderr:\n{compile_result['stderr']}")
        return {"name": test_name, "passed": passed, "messages": messages}

    asm_code = compile_result["stdout"]

    if expected["stdout_match"]:
        if not re.search(expected["stdout_match"], asm_code, re.MULTILINE):
            passed = False
            messages.append(f"STDOUT_MATCH failed. Pattern: {expected['stdout_match']}")

    if expected["stderr_match"]:
        if not re.search(expected["stderr_match"], compile_result["stderr"], re.MULTILINE):
            passed = False
            messages.append(f"STDERR_MATCH failed. Pattern: {expected['stderr_match']}")

    if expected["exit_code"] is not None:
        run_result = run_assembly(asm_code, test_name, temp_dir)

        if run_result["emulator"] is None:
            messages.append("No RISC-V emulator found (qemu-riscv32 or spike). Skipping execution.")
        elif run_result["exit_code"] is None:
            messages.append(f"Could not determine exit code from {run_result['emulator']}")
        elif run_result["exit_code"] != expected["exit_code"]:
            passed = False
            messages.append(f"Expected exit code {expected['exit_code']}, got {run_result['exit_code']}")

    return {"name": test_name, "passed": passed, "messages": messages}


def main():
    parser = argparse.ArgumentParser(description="Run ToyC compiler tests")
    parser.add_argument("test_path", nargs="?", default="tests/", help="Test directory or file")
    parser.add_argument("--opt", action="store_true", help="Enable optimizer")
    parser.add_argument("--build-dir", default="build", help="Build directory")
    parser.add_argument("--temp-dir", default="tests/.tmp", help="Temporary directory")
    parser.add_argument("--no-build", action="store_true", help="Skip rebuild")
    args = parser.parse_args()

    test_path = args.test_path
    opt = args.opt
    build_dir = args.build_dir
    temp_dir = args.temp_dir

    os.makedirs(temp_dir, exist_ok=True)

    if not args.no_build:
        print(f"Building compiler...")
        compiler_path = build_compiler(build_dir, opt)
        if not compiler_path:
            sys.exit(1)
        print(f"Compiler built: {compiler_path}")
    else:
        compiler_path = os.path.join(build_dir, "toyc.exe" if sys.platform == "win32" else "toyc")
        if not os.path.exists(compiler_path):
            print(f"Compiler not found at {compiler_path}. Build first or use --no-build.")
            sys.exit(1)

    tc_files = []
    if os.path.isfile(test_path) and test_path.endswith(".tc"):
        tc_files = [test_path]
    elif os.path.isdir(test_path):
        for root, dirs, files in os.walk(test_path):
            for f in files:
                if f.endswith(".tc"):
                    tc_files.append(os.path.join(root, f))
    else:
        print(f"Invalid test path: {test_path}")
        sys.exit(1)

    tc_files.sort()

    print(f"\nFound {len(tc_files)} test(s)\n")

    passed_count = 0
    failed_count = 0

    for tc_file in tc_files:
        rel_path = os.path.relpath(tc_file, "tests")
        print(f"Running: {rel_path}")

        result = run_test(compiler_path, tc_file, temp_dir, opt)

        if result["passed"]:
            print(f"  PASS")
            passed_count += 1
        else:
            print(f"  FAIL")
            for msg in result["messages"]:
                print(f"    - {msg}")
            failed_count += 1

    print(f"\n{'='*40}")
    print(f"Results: {passed_count} passed, {failed_count} failed")

    if failed_count > 0:
        sys.exit(1)


if __name__ == "__main__":
    main()