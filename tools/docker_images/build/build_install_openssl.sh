#!/usr/bin/env bash

set -e

VERSION=$1

# Remove existing openssl
sudo apt-get purge -y \
    openssl \
    libssl-dev

# Install deps
sudo apt-get install -y --no-install-recommends \
    libtool \
    perl \
    zlib1g-dev

curl -kL https://github.com/openssl/openssl/archive/refs/tags/OpenSSL_${VERSION}.tar.gz | tar -xz
pushd openssl-OpenSSL_${VERSION}

./config --prefix=/usr shared zlib
make -j$(nproc)
make install

# Update the certificates
sudo apt-get install -y --no-install-recommends \
    ca-certificates

popd

rm -rf openssl-OpenSSL_${VERSION}
