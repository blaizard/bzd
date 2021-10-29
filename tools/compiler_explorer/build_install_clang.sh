#!/usr/bin/env bash

set -e

# https://llvm.org/docs/CMake.html
# https://libcxx.llvm.org/docs/BuildingLibcxx.html
# https://github.com/llvm/llvm-project

VERSION=$1

curl -L https://github.com/llvm/llvm-project/releases/download/llvmorg-${VERSION}/llvm-project-${VERSION}.src.tar.xz | tar -xJ

pushd llvm-project-${VERSION}.src

cmake -S llvm -B build -G "Unix Makefiles" -DLLVM_ENABLE_PROJECTS="clang" -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi" -DCMAKE_BUILD_TYPE=Release
cd build
make -j$(getconf _NPROCESSORS_ONLN)
make install

popd

rm -rfd llvm-project-${VERSION}.src
