#!/usr/bin/env python3
"""生成 SysY 源文件：枚举多组 (n,d)，用 putint 输出 n/d 与 n%d（与 C99 向零取整除法一致）。"""
import sys


def int32(x: int) -> int:
    x &= 0xFFFFFFFF
    if x >= 0x80000000:
        x -= 0x100000000
    return x


def trunc_div(n: int, d: int) -> int:
    n, d = int32(n), int32(d)
    if d == 0:
        return 0
    neg = (n < 0) ^ (d < 0)
    an, ad = abs(n), abs(d)
    q = an // ad
    q = q if not neg else -q
    return int32(q)


def trunc_mod(n: int, d: int) -> int:
    return int32(n - trunc_div(n, d) * int32(d))


def main() -> None:
    out = sys.stdout
    if len(sys.argv) > 1:
        out = open(sys.argv[1], "w", encoding="utf-8")

    ns = [
        0,
        1,
        -1,
        42,
        -42,
        100,
        -100,
        2147483647,
        -2147483647,
        -2147483648,
        -2147483647 - 1,
    ]
    ds = [1, 2, -2, 3, 7, -7, 13, -13, 1024, -1024, 4, -4]

    pairs = []
    for n in ns:
        for d in ds:
            if d == 0:
                continue
            pairs.append((n, d, trunc_div(n, d), trunc_mod(n, d)))

    print("int main() {", file=out)
    for n, d, q, r in pairs:
        print(f"  putint({n}); putch(32); putint({d}); putch(32);", file=out)
        print(f"  putint({q}); putch(32); putint({r}); putch(10);", file=out)
    print("  return 0;", file=out)
    print("}", file=out)

    if out is not sys.stdout:
        out.close()


if __name__ == "__main__":
    main()
