#!/bin/bash

set -e

VERSION=15.0.0
HOST=linux_x86_64
PACKAGE=${HOST}_${VERSION}

curl -L https://github.com/llvm/llvm-project/releases/download/llvmorg-${VERSION}/llvm-project-${VERSION}.src.tar.xz | tar -xJ
pushd llvm-project-${VERSION}.src
cmake -G "Unix Makefiles" -S llvm -B build -DCMAKE_BUILD_TYPE=Release \
                                -DCMAKE_INSTALL_PREFIX="$(pwd)/build/${PACKAGE}" \
                                -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;lld" \
                                -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libunwind" \
                                -DLLVM_RUNTIME_TARGETS="x86_64-unknown-linux-gnu"
cd build
make -j$(getconf _NPROCESSORS_ONLN)
make install

# Create an archive.
tar -cvJf ${PACKAGE}.tar.xz ../${PACKAGE}
echo "Archive successfully built at $(pwd)/${PACKAGE}.tar.xz"

popd
