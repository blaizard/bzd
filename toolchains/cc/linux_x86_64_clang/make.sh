#!/bin/bash

set -e

VERSION=14.0.6
HOST=linux_x86_64
PACKAGE=${HOST}_${VERSION}

rm -rfd llvm-project-${VERSION}.src
curl -L https://github.com/llvm/llvm-project/releases/download/llvmorg-${VERSION}/llvm-project-${VERSION}.src.tar.xz | tar -xJ
pushd llvm-project-${VERSION}.src
cmake -G "Unix Makefiles" -S llvm -B build -DCMAKE_BUILD_TYPE="Release" \
                                -DCMAKE_INSTALL_PREFIX="$(pwd)/build/${PACKAGE}" \
                                -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;lld" \
                                -DLLVM_ENABLE_RUNTIMES="compiler-rt;libcxx;libcxxabi;libunwind" \
                                -DLLVM_TARGETS_TO_BUILD="host" \
                                -DLLVM_RUNTIME_TARGETS="x86_64-unknown-linux-gnu" \
                                -DLLVM_INCLUDE_TESTS=Off \
                                -DLLVM_INCLUDE_EXAMPLES=Off \

cd build
make -j4 #$(nproc)
make install

# Create an archive.
tar -cvJf ${PACKAGE}.tar.xz ${PACKAGE}
echo "Archive successfully built at $(pwd)/${PACKAGE}.tar.xz"

popd
