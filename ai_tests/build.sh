#!/usr/bin/env bash
# Builds and runs the AI behavior test suite. Plain gcc/clang, no meson, no
# NDS toolchain -- this is never part of the ROM build. See README.md.
set -euo pipefail
cd "$(dirname "$0")"

CC="${CC:-cc}"
OUT="ai_tests_run"

# shellcheck disable=SC2206
SRC=(testing.c ai_model.c main.c scenarios/*.c)

"$CC" -std=c11 -Wall -Wextra -O0 -g -o "$OUT" "${SRC[@]}"
./"$OUT"
