#!/usr/bin/env python3
import pathlib
import random
import subprocess
import sys

from run_regressions import Machine

ROOT = pathlib.Path(__file__).resolve().parent


def compile_source(compiler, source, args):
    result = subprocess.run(
        [str(compiler), *args],
        input=source,
        text=True,
        capture_output=True,
        check=False,
    )
    if result.returncode != 0:
        raise AssertionError(result.stderr.strip())
    return result.stdout


def make_program(rng, index):
    initial_g = rng.randint(-9, 9)
    initial_total = rng.randint(-20, 20)
    iterations = rng.randint(3, 40)
    divisor = rng.randint(2, 7)
    threshold = rng.randint(1, divisor - 1)
    helper_delta = rng.randint(1, 8)
    linear_scale = rng.randint(-4, 6)
    linear_bias = rng.randint(-8, 8)
    tail_count = rng.randint(1, 30)

    return f"""int g = {initial_g};

int helper(int a, int b) {{
    if (a % 3 == 0) {{
        g = g + b;
        return g + a;
    }}
    g = g - a;
    return g + b;
}}

int tail(int n, int acc) {{
    if (n == 0) {{
        return acc;
    }}
    return tail(n - 1, acc + n);
}}

int main() {{
    int i = 0;
    int total = {initial_total};
    while (i < {iterations}) {{
        if (i % {divisor} < {threshold}) {{
            total = total + helper(i, {helper_delta});
        }} else {{
            total = total + i * {linear_scale} + {linear_bias};
        }}
        if (total % 5 == 0) {{
            g = g + 1;
        }}
        i = i + 1;
    }}
    return total + tail({tail_count}, {index}) + g;
}}
"""


def make_affine_declaration_program(rng):
    bound = rng.randint(1024, 1800)
    initial_x = rng.randint(-20, 20)
    initial_y = rng.randint(-20, 20)
    term_scale = rng.randint(-4, 6)
    term_bias = rng.randint(-8, 8)
    factor = rng.randint(-2, 3)
    mix_scale = rng.randint(-3, 4)

    return f"""int main() {{
    int i = 0;
    int x = {initial_x};
    int y = {initial_y};
    while (i < {bound}) {{
        int term = i * {term_scale} + {term_bias};
        const int factor = {factor};
        int next = x * factor + term;
        next = next + y;
        x = next;
        int mix = x + y * {mix_scale} - i;
        y = mix;
        i = i + 1;
    }}
    return x + y + i;
}}
"""


def main():
    compiler = pathlib.Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT.parent / "build" / "toyc.exe"
    rng = random.Random(0xC0DEC0DE)
    failures = []

    for index in range(40):
        source = make_program(rng, index)
        try:
            normal_asm = compile_source(compiler, source, [])
            backend_asm = compile_source(compiler, source, ["-opt", "-fno-ctfe"])
            ctfe_asm = compile_source(compiler, source, ["-opt"])

            normal = Machine(normal_asm).run()
            backend = Machine(backend_asm).run()
            ctfe_machine = Machine(ctfe_asm)
            ctfe = ctfe_machine.run()

            if normal != backend or backend != ctfe:
                failures.append(
                    f"case {index}: normal={normal}, backend={backend}, ctfe={ctfe}"
                )
            if len(ctfe_machine.instructions) != 2:
                failures.append(
                    f"case {index}: CTFE emitted {len(ctfe_machine.instructions)} instructions"
                )
        except Exception as error:
            failures.append(f"case {index}: {error}")

    for index in range(30):
        source = make_affine_declaration_program(rng)
        try:
            normal_asm = compile_source(compiler, source, [])
            backend_asm = compile_source(compiler, source, ["-opt", "-fno-ctfe"])
            ctfe_asm = compile_source(compiler, source, ["-opt"])

            normal = Machine(normal_asm).run()
            backend = Machine(backend_asm).run()
            ctfe_machine = Machine(ctfe_asm)
            ctfe = ctfe_machine.run()

            if normal != backend or backend != ctfe:
                failures.append(
                    f"affine declaration case {index}: normal={normal}, backend={backend}, ctfe={ctfe}"
                )
            if len(ctfe_machine.instructions) != 2:
                failures.append(
                    f"affine declaration case {index}: CTFE emitted {len(ctfe_machine.instructions)} instructions"
                )
        except Exception as error:
            failures.append(f"affine declaration case {index}: {error}")

    if failures:
        print("\n".join(failures), file=sys.stderr)
        return 1
    print("passed 70 deterministic CTFE differential programs")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
