#!/bin/bash

set -e

# Best, get the packages from: https://github.com/indygreg/python-build-standalone

VERSION=3.8.17
HOST=linux_x86_64
PACKAGE=${HOST}_${VERSION}

sudo apt install -y libffi-dev

rm -rf cpython-${VERSION} cpython-${VERSION}-build
curl -L https://github.com/python/cpython/archive/refs/tags/v${VERSION}.tar.gz | tar -xz

mkdir cpython-${VERSION}-build
pushd cpython-${VERSION}-build

../cpython-${VERSION}/configure --prefix="$(pwd)/../${PACKAGE}" \
        --enable-optimizations \
        --with-lto \
        --with-system-ffi
make -j$(nproc)
make altinstall

popd

# Create an archive.
tar -cvJf ${PACKAGE}.tar.xz ${PACKAGE}
echo "Archive successfully built at $(pwd)/${PACKAGE}.tar.xz"
