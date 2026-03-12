#!/bin/bash

set -e

TAG="esp-develop-9.2.2-20250817"
HOST=linux_x86_64
PACKAGE=${HOST}_${TAG}
INSTALL=$(pwd)/${PACKAGE}

# ---- Pre-requisites ----

sudo apt install -y \
    git \
    make \
    ninja-build \
    build-essential \
    pkg-config \
    libgcrypt20-dev \
    libpixman-1-dev \
    libffi-dev

pip3 install --user meson ninja
export PATH="$HOME/.local/bin:$PATH"

# ---- Setup ----

git clone --depth 1 --branch ${TAG} --recurse-submodules https://github.com/espressif/qemu.git qemu
git clone --depth 1 --branch "2.82.0" https://gitlab.gnome.org/GNOME/glib.git glib
git clone --depth 1 https://gitlab.freedesktop.org/slirp/libslirp.git

# ---- slirp ----

pushd libslirp
meson setup _build --default-library=static
meson compile -C _build
meson install -C _build
popd

# ---- GLIB ----

pushd glib
meson setup _build --default-library=static
meson compile -C _build
meson install -C _build
popd

# ---- QEMU ----

mkdir -p qemu/build
pushd qemu/build
../configure --target-list=xtensa-softmmu,riscv32-softmmu \
            --static \
            --prefix="${INSTALL}"

# ---- Build/Install ----

make -j $(nproc) vga=no
make install
popd

# ---- Set the RPATH ----

python3 patchall.py "${INSTALL}"

# ---- Create an archive ----

tar -cvJf ${PACKAGE}.tar.xz ${PACKAGE}
echo "Archive successfully built at $(pwd)/${PACKAGE}.tar.xz"
