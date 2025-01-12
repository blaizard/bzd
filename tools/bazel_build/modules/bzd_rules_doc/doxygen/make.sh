#!/bin/bash

set -e

VERSION=1_13_2
HOST=linux_x86_64
PACKAGE=${HOST}_${VERSION}
INSTALL=$(pwd)/${PACKAGE}

# ---- Setup ----

sudo apt install -y \
    git \
    flex \
    bison

mkdir -p build ${INSTALL}

export CC=/usr/local/bin/gcc-static
export CXX=/usr/local/bin/g++-static

# ---- Get the sources ----

git clone --depth 1 https://github.com/doxygen/doxygen.git -b "Release_${VERSION}"

# ---- Build llvm-project ----

pushd doxygen
cmake -G "Unix Makefiles" -B ../build -DCMAKE_BUILD_TYPE="Release" \
                                -DCMAKE_INSTALL_PREFIX="${INSTALL}"

cd ../build
make -j$(nproc)
make install
popd

# ---- Strip all ----

find "${INSTALL}" -type f -executable | grep -vF '.a' | grep -vF '.la' | xargs -n1 strip

# ---- Set the RPATH ----

patchall.py "${INSTALL}"

# ---- Create an archive ----

tar -cvJf ${PACKAGE}.tar.xz ${PACKAGE}
echo "Archive successfully built at $(pwd)/${PACKAGE}.tar.xz"
