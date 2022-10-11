#!/bin/bash

set -e

VERSION=11.2.0
HOST=linux_x86_64
PACKAGE=${HOST}_${VERSION}

# https://gcc.gnu.org/mirrors.html
curl -L https://ftp.gnu.org/gnu/gcc/gcc-${VERSION}/gcc-${VERSION}.tar.xz | tar -xJ

mkdir gcc-${VERSION}-build
pushd gcc-${VERSION}-build

../gcc-${VERSION}/configure --prefix="$(pwd)/${PACKAGE}" --enable-languages=c,c++
make -j$(getconf _NPROCESSORS_ONLN)
make install

# Create an archive.
tar -cvJf ${PACKAGE}.tar.xz ${PACKAGE}
echo "Archive successfully built at $(pwd)/${PACKAGE}.tar.xz"

popd
