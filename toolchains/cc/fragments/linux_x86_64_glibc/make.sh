#!/bin/bash

set -e

VERSION=2.24
HOST=linux_x86_64
PACKAGE=${HOST}_${VERSION}

rm -rfd glibc-${VERSION}
curl -L https://ftp.gnu.org/gnu/glibc/glibc-${VERSION}.tar.xz | tar -xJ
pushd glibc-${VERSION}

mkdir build && cd build
../configure --prefix="$(pwd)/${PACKAGE}" CFLAGS="-Wno-error -O3" CXXFLAGS="-Wno-error -O3"
make -j4 #$(nproc)
make install

# Create an archive.
tar -cvJf ${PACKAGE}.tar.xz ${PACKAGE}
echo "Archive successfully built at $(pwd)/${PACKAGE}.tar.xz"

popd
