#!/usr/bin/env bash

set -e

VERSION=$1

sudo apt-get install -y --no-install-recommends \
    git \
    python3

curl -L https://github.com/llvm/llvm-project/releases/download/llvmorg-${VERSION}/llvm-project-${VERSION}.src.tar.xz | tar -xJ
pushd "llvm-project-${VERSION}.src"
cmake -G "Unix Makefiles" -S llvm -B build -DCMAKE_BUILD_TYPE="Release" \
                                -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;lld" \
                                -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libunwind" \
                                -DLLVM_TARGETS_TO_BUILD="X86" \
                                -DLLVM_INCLUDE_TESTS=Off \
                                -DLLVM_INCLUDE_BENCHMARKS=Off \
                                -DLLVM_INCLUDE_EXAMPLES=Off

cd build
make -j$(nproc)
make install
popd

rm -rfd "llvm-project-${VERSION}.src"
