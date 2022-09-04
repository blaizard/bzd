#!/usr/bin/env bash

set -e

# https://github.com/llvm/llvm-project

VERSION=$1

curl -L https://github.com/llvm/llvm-project/releases/download/llvmorg-${VERSION}/llvm-project-${VERSION}.src.tar.xz | tar -xJ

pushd llvm-project-${VERSION}.src

cmake -S llvm -B build -G "Unix Makefiles"
cd build
make -j$(getconf _NPROCESSORS_ONLN)
make install

popd

rm -rfd llvm-project-${VERSION}.src
