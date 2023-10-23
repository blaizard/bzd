#!/usr/bin/env bash

set -e

VERSION=$1
INSTALL=/usr

# Remove existing openssl
sudo apt-get purge -y \
    openssl \
    libssl-dev

# Install deps
sudo apt-get install -y --no-install-recommends \
    libtool \
    perl \
    zlib1g-dev

git clone https://github.com/openssl/openssl.git -b "openssl-$VERSION" openssl

pushd openssl

./config --prefix=$INSTALL --openssldir=$INSTALL shared zlib
make -j$(nproc)
make install

# Update the certificates
sudo apt-get install -y --no-install-recommends \
    ca-certificates

popd

rm -rf openssl-OpenSSL_${VERSION}
