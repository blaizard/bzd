#!/bin/bash

set -e

GCC_VERSION=14.2.0
HOST=linux_x86_64
PACKAGE=${HOST}_${GCC_VERSION}
INSTALL="$(pwd)/${PACKAGE}"

# ---- Setup ----

sudo apt install -y \
    git \
    automake \
    texinfo \
    bison \
    flex

mkdir -p srcs build/{isl,mpfr,gmp,mpc,gcc,binutils} ${INSTALL}

export PATH="${INSTALL}/bin:${PATH}"
export LD_LIBRARY_PATH="${INSTALL}/lib:${LD_LIBRARY_PATH}"

# ---- Get the sources ----

pushd srcs
git clone --depth 1 https://repo.or.cz/isl.git -b "isl-0.26"
git clone --depth 1 https://gitlab.inria.fr/mpfr/mpfr.git -b "4.2"
curl -L https://gmplib.org/download/gmp/gmp-6.2.1.tar.xz | tar -xJ --transform 's/gmp[^\/]*/gmp/'
git clone --depth 1 https://gitlab.inria.fr/mpc/mpc.git -b "1.3.1"
git clone --depth 1 https://github.com/gcc-mirror/gcc.git -b "releases/gcc-${GCC_VERSION}"
git clone --depth 1 https://sourceware.org/git/binutils-gdb.git -b "binutils-2_39" binutils
popd

# ---- Build isl ----

(cd srcs/isl; ./autogen.sh)
pushd build/isl
../../srcs/isl/configure --prefix="${INSTALL}"
make -j$(nproc)
make install
popd

# ---- Build mpfr ----

(cd srcs/mpfr; ./autogen.sh)
pushd build/mpfr
../../srcs/mpfr/configure --prefix="${INSTALL}"
make -j$(nproc)
make install
popd

# ---- Build gmp ----

pushd build/gmp
../../srcs/gmp/configure --prefix="${INSTALL}"
make -j$(nproc)
make install
popd

# ---- Build mpc ----

(cd srcs/mpc; autoreconf -i)
pushd build/mpc
../../srcs/mpc/configure \
    --prefix="${INSTALL}" \
    --with-gmp="${INSTALL}" \
    --with-mpfr="${INSTALL}"
make -j$(nproc)
make install
popd

# ---- Build gcc ----

pushd build/gcc
../../srcs/gcc/configure \
    --prefix="${INSTALL}" \
    --disable-multilib \
    --disable-bootstrap \
    --disable-nls \
    --enable-lto \
    --enable-languages=c,c++,lto \
    --with-isl="${INSTALL}" \
    --with-gmp="${INSTALL}" \
    --with-mpfr="${INSTALL}" \
    --with-mpc="${INSTALL}"
make -j$(nproc)
make install
popd

# ---- Build binutils ----

pushd build/binutils
../../srcs/binutils/configure \
    --prefix="${INSTALL}" \
    --disable-multilib \
    --disable-bootstrap \
    --disable-nls \
    --enable-lto \
    --with-isl="${INSTALL}" \
    --with-gmp="${INSTALL}" \
    --with-mpfr="${INSTALL}" \
    --with-mpc="${INSTALL}"
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
