#!/bin/bash

set -e

GCC_VERSION=11.2.0
BINUTILS_VERSION=2.39
HOST=linux_x86_64
PACKAGE=${HOST}_${GCC_VERSION}

# --- Compile binutils ----

# https://ftp.gnu.org/gnu/binutils/
rm -rfd binutils-${BINUTILS_VERSION}
curl -L https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz | tar -xJ

mkdir binutils-${BINUTILS_VERSION}-build
pushd binutils-${BINUTILS_VERSION}-build

../binutils-${BINUTILS_VERSION}/configure --prefix="$(pwd)/../${PACKAGE}"
make -j$(getconf _NPROCESSORS_ONLN)
make install

popd

# --- Compile GCC ----

# https://gcc.gnu.org/mirrors.html
rm -rfd gcc-${GCC_VERSION}
curl -L https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz | tar -xJ

mkdir gcc-${GCC_VERSION}-build
pushd gcc-${GCC_VERSION}-build

../gcc-${GCC_VERSION}/configure --prefix="$(pwd)/../${PACKAGE}" --disable-multilib --enable-languages=c,c++,lto
make -j$(nproc)
make install

popd

# Create an archive.
tar -cvJf ${PACKAGE}.tar.xz ${PACKAGE}
echo "Archive successfully built at $(pwd)/${PACKAGE}.tar.xz"
