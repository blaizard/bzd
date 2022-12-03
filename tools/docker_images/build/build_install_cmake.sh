#!/usr/bin/env bash

set -e

VERSION=$1

. ~/.bashrc

sudo apt-get install -y --no-install-recommends \
    curl \
    xz-utils \
    build-essential \
    libtool \
    autoconf \
    ca-certificates \
    openssl \
    libssl-dev

curl -L https://github.com/Kitware/CMake/archive/refs/tags/v${VERSION}.tar.gz | tar -xz
pushd CMake-${VERSION}

./bootstrap
make -j$(nproc)
make install

popd

rm -rf CMake-${VERSION}
