#!/usr/bin/env bash

set -e

# https://gcc.gnu.org/wiki/InstallingGCC

VERSION=$1
INSTALL_PATH=/usr/local

. ~/.bashrc

sudo apt install -y --no-install-recommends \
    curl \
    build-essential \
    g++-multilib \
    xz-utils \
    libgmp-dev \
    libmpfr-dev \
    libmpc-dev \
    libisl-dev

curl ftp://gcc.gnu.org/pub/gcc/releases/gcc-${VERSION}/gcc-${VERSION}.tar.gz | tar -xz
mkdir gcc-${VERSION}-build
pushd gcc-${VERSION}-build

../gcc-${VERSION}/configure \
    --prefix=${INSTALL_PATH} \
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
echo "export PATH=${INSTALL_PATH}/bin:${PATH}" >> ~/.bashrc
echo "export LD_LIBRARY_PATH=${INSTALL_PATH}/lib64:${LD_LIBRARY_PATH}" >> ~/.bashrc
echo "export CC=${INSTALL_PATH}/bin/gcc-static" >> ~/.bashrc
echo "export CXX=${INSTALL_PATH}/bin/g++-static" >> ~/.bashrc

popd

rm -rfd gcc-${VERSION}
rm -rfd gcc-${VERSION}-build
