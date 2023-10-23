#!/bin/bash

set -e

VERSION="8.1.2"
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

git clone https://github.com/espressif/qemu.git -b "esp-develop" qemu

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
