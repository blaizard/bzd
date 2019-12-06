#!/bin/bash

gdb -tui -iex "layout asm" -iex "layout regs" -ex "b main" -ex "r" "$1"

# Usefull commands

# Show 10x 64-bit elements from the stack
#x/10xg $sp
#x/10xg $rbp

# Disassemble
# objdump -drwCS -Mintel

# 0x555555557828 <temp1> 