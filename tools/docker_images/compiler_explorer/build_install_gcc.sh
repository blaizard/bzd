#!/usr/bin/env bash

set -e

# https://gcc.gnu.org/wiki/InstallingGCC

VERSION=$1

curl ftp://gcc.gnu.org/pub/gcc/releases/gcc-${VERSION}/gcc-${VERSION}.tar.gz | tar -xz
mkdir gcc-${VERSION}-build
pushd gcc-${VERSION}-build

../gcc-${VERSION}/configure --prefix=/usr/local/gcc/gcc-${VERSION} --enable-languages=c,c++
make -j$(getconf _NPROCESSORS_ONLN)
make install

popd

rm -rfd gcc-${VERSION}
rm -rfd gcc-${VERSION}-build
