#!/usr/bin/env bash

set -e

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

cd /tmp/
curl https://www.python.org/ftp/python/${VERSION}/Python-${VERSION}.tgz | tar xz
pushd Python-${VERSION}

./configure --prefix=${INSTALL_PATH} --enable-optimizations --with-lto --with-computed-gotos --with-system-ffi
make -j$(nproc)
make altinstall
popd

rm -rfd /tmp/Python-${VERSION}
