#!/bin/bash

set -e

VERSION=3.8.15
HOST=linux_x86_64
PACKAGE=${HOST}_${VERSION}

rm -rfd Python-${VERSION}
curl -L https://www.python.org/ftp/python/${VERSION}/Python-${VERSION}.tar.xz | tar -xJ

rm -rfd Python-${VERSION}-build
mkdir Python-${VERSION}-build
pushd Python-${VERSION}-build

../Python-${VERSION}/configure --prefix="$(pwd)/../${PACKAGE}"
make -j$(getconf _NPROCESSORS_ONLN)
make install

popd

# Create an archive.
tar -cvJf ${PACKAGE}.tar.xz ${PACKAGE}
echo "Archive successfully built at $(pwd)/${PACKAGE}.tar.xz"
