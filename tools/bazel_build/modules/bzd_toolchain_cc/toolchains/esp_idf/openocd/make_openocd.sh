#!/bin/bash

set -e

VERSION="v0.12.0-esp32-20241016"
HOST=linux_x86_64
PACKAGE=${HOST}_${VERSION}
INSTALL=$(pwd)/${PACKAGE}

# ---- Pre-requisites ----

sudo apt install -y \
    git \
    autotools-dev \
    automake \
    pkg-config \
    libusb-1.0-0-dev

# ---- Setup ----

git clone https://github.com/espressif/openocd-esp32.git --branch "v0.12.0-esp32-20241016" openocd

# ---- QEMU ----

mkdir -p openocd/build
cd openocd
./bootstrap
cd build
../configure --prefix="${INSTALL}"

# ---- Build/Install ----

make -j $(nproc)
make install
cd ../..

# ---- Set the RPATH ----

patchall.py "${INSTALL}"

# ---- Create an archive ----

tar -cvJf ${PACKAGE}.tar.xz ${PACKAGE}
echo "Archive successfully built at $(pwd)/${PACKAGE}.tar.xz"
