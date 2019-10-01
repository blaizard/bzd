#!/bin/bash

root=/opt/toolchains/linux_x86_64/
mkdir -p "$root" && cd "$root"

version="7.0.0"
#curl "http://releases.llvm.org/${version}/clang+llvm-${version}-x86_64-linux-gnu-ubuntu-16.04.tar.xz" | tar xJ
#mv "clang+llvm-${version}-x86_64-linux-gnu-ubuntu-16.04" "clang-${version}"


curl "http://ftp.gnu.org/gnu/libc/glibc-2.30.tar.xz" | tar xJ
