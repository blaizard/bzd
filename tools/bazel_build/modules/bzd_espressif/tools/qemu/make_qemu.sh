#!/bin/bash

set -e

VERSION="20240219"
HOST=linux_x86_64
PACKAGE=${HOST}_${VERSION}
INSTALL=$(pwd)/${PACKAGE}

# ---- Pre-requisites ----

sudo apt install -y \
    git \
    make \
    ninja-build \
    build-essential \
    pkg-config \
    libgcrypt20-dev \
    libglib2.0-dev \
    libpixman-1-dev

# ---- Setup ----

git clone --recurse-submodules https://github.com/espressif/qemu.git qemu

# ---- QEMU ----

mkdir -p qemu/build
pushd qemu/build
../configure --target-list=xtensa-softmmu \
            --static \
            --prefix="${INSTALL}"

# ---- Build/Install ----

make -j $(nproc) vga=no
make install
popd

# ---- Set the RPATH ----

patchall.py "${INSTALL}"

# ---- Create an archive ----

tar -cvJf ${PACKAGE}.tar.xz ${PACKAGE}
echo "Archive successfully built at $(pwd)/${PACKAGE}.tar.xz"
