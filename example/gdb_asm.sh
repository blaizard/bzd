#!/bin/bash

gdb -tui -iex "layout asm" -iex "layout regs" -ex "b main" -ex "r" "$1"

# Useful commands

# Show 10x 64-bit elements from the stack
#x/10xg $sp
#x/10xg $rbp

# Disassemble
# objdump -drwCS -Mintel

# 0x555555557828 <temp1> 

# Profile size of binary (function sizes)
# nm --demangle --print-size --size-sort --reverse-sort bazel-bin/example/bzd/bzd.binary

# File size
# objdump --dwarf=info bazel-bin/example/bzd/bzd.binary
# Look for DW_TAG_compile_unit
# https://www.ibm.com/developerworks/aix/library/au-unixtools.html