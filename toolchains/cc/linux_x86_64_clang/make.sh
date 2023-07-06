#!/bin/bash

set -e

VERSION=16.0.6
HOST=linux_x86_64
PACKAGE=${HOST}_${VERSION}
INSTALL=$(pwd)/${PACKAGE}

# ---- Setup ----

sudo apt install -y \
    git \
    python3

mkdir -p build ${INSTALL}

export CC=/usr/local/bin/clang
export CXX=/usr/local/bin/clang++

# ---- Get the sources ----

git clone --depth 1 https://github.com/llvm/llvm-project.git -b "llvmorg-${VERSION}"

# ---- Build llvm-project ----

pushd llvm-project
cmake -G "Unix Makefiles" -S llvm -B ../build -DCMAKE_BUILD_TYPE="Release" \
                                -DCMAKE_INSTALL_PREFIX="${INSTALL}" \
                                -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;lld" \
                                -DLLVM_ENABLE_RUNTIMES="compiler-rt;libcxx;libcxxabi;libunwind" \
                                -DLLVM_TARGETS_TO_BUILD="X86" \
                                -DLLVM_ENABLE_LIBCXX=On \
                                -DLIBCXXABI_USE_COMPILER_RT=On \
                                -DLIBCXXABI_USE_LLVM_UNWINDER=On \
                                -DLIBCXX_USE_COMPILER_RT=On \
                                -DLLVM_EXTERNALIZE_DEBUGINFO=On \
                                -DLLVM_INCLUDE_TESTS=Off \
                                -DLLVM_INCLUDE_BENCHMARKS=Off \
                                -DLLVM_INCLUDE_EXAMPLES=Off

# See https://clang.llvm.org/docs/Toolchain.html#compiler-rt-llvm
#     https://clang.llvm.org/docs/Toolchain.html#libunwind-llvm

# Note, when it fails, use:
# CPLUS_INCLUDE_PATH=/sandbox/build/include/c++/v1/:/sandbox/build/include/x86_64-unknown-linux-gnu/c++/v1/ make

cd ../build
make -j$(nproc)
make install
popd

# ---- Strip all ----

find "${INSTALL}" -type f -executable | grep -vF '.a' | grep -vF '.la' | xargs -n1 strip

# ---- Set the RPATH ----

./toolchains/cc/linux_x86_64_gcc/patchall.py "${INSTALL}"

# ---- Create an archive ----

tar -cvJf ${PACKAGE}.tar.xz ${PACKAGE}
echo "Archive successfully built at $(pwd)/${PACKAGE}.tar.xz"
