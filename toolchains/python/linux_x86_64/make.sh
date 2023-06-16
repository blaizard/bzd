#!/bin/bash

set -e

# Best, get the packages from: https://github.com/indygreg/python-build-standalone

VERSION=3.8.15
HOST=linux_x86_64
PACKAGE=${HOST}_${VERSION}

rm -rf Python-${VERSION}
curl -L https://www.python.org/ftp/python/${VERSION}/Python-${VERSION}.tar.xz | tar -xJ

rm -rf Python-${VERSION}-build
mkdir Python-${VERSION}-build
pushd Python-${VERSION}-build

../Python-${VERSION}/configure --prefix="$(pwd)/../${PACKAGE}"
make -j$(nproc)
make install

popd

# Create an archive.
tar -cvJf ${PACKAGE}.tar.xz ${PACKAGE}
echo "Archive successfully built at $(pwd)/${PACKAGE}.tar.xz"
