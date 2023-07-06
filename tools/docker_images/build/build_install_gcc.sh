#!/usr/bin/env bash

set -e

# https://gcc.gnu.org/wiki/InstallingGCC

# Default to /usr instead of /usr/local, this will install it along side other gcc distro, which
# is needed for clang to find installed gcc version. This is a requisite when build llvm-project for example.
INSTALL_PATH=/usr
while [[ $# -gt 0 ]] && [[ "$1" == "--"* ]] ;
do
    opt="$1";
    shift;
    case "$opt" in
        "--install" )
            INSTALL_PATH="$1"; shift;;
        *) echo >&2 "Invalid option: $@"; exit 1;;
   esac
done
VERSION=$1


sudo apt-get install -y --no-install-recommends \
    curl \
    file \
    build-essential \
    g++-multilib \
    xz-utils \
    libgmp-dev \
    libmpfr-dev \
    libmpc-dev \
    libisl-dev

curl -L ftp://gcc.gnu.org/pub/gcc/releases/gcc-${VERSION}/gcc-${VERSION}.tar.gz | tar -xz
mkdir gcc-${VERSION}-build
pushd gcc-${VERSION}-build

../gcc-${VERSION}/configure \
    --disable-bootstrap \
    --disable-nls \
    --enable-languages=c,c++

make -j$(nproc)
make install

# Create a g++ wrapper to build static binaries.
cat <<EOT > "${INSTALL_PATH}/bin/gcc-static" && chmod +x "${INSTALL_PATH}/bin/gcc-static"
#!/bin/bash
${INSTALL_PATH}/bin/gcc -static-libgcc "\$@"
EOT
# Create a gcc wrapper to build static binaries.
cat <<EOT > "${INSTALL_PATH}/bin/g++-static" && chmod +x "${INSTALL_PATH}/bin/g++-static"
#!/bin/bash
${INSTALL_PATH}/bin/g++ -static-libgcc -static-libstdc++ "\$@"
EOT

popd

rm -rf gcc-${VERSION}
rm -rf gcc-${VERSION}-build
