#!/bin/bash
set -eu

worker="worker-linux_x86_64"

cd "tools/buildbarn"
sudo rm -rf bb "${worker}"
mkdir -m 0777 "${worker}" "${worker}/build"
mkdir -m 0700 "${worker}/cache"
mkdir -m 0700 -p storage-{ac,cas}-{0,1}/persistent_state

echo "===================================================="
echo "Scheduler: http://localhost:7982/"
echo "===================================================="

exec docker-compose up "$@"
