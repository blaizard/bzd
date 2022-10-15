#!/bin/bash
set -eu

SCRIPT_PATH=$(dirname "$(realpath -s "$0")")
export STORAGE_PATH=$(realpath -s "/tmp/buildbarn")
export WORKER_STORAGE_PATH=$(realpath -s "/tmp/buildbarn-worker-linux_x86_64")

echo "===================================================="
echo "Config: ${SCRIPT_PATH}/config                       "
echo "Storage: ${STORAGE_PATH}                            "
echo "Scheduler: http://localhost:7982/                   "
echo "===================================================="

cd "${SCRIPT_PATH}"
sudo rm -rf "${WORKER_STORAGE_PATH}"
mkdir -m 0777 "${WORKER_STORAGE_PATH}" "${WORKER_STORAGE_PATH}/build" "${WORKER_STORAGE_PATH}/bb"
mkdir -m 0700 "${WORKER_STORAGE_PATH}/cache"
rm -rfd ${STORAGE_PATH}/storage-{ac,cas}-0
mkdir -m 0700 -p ${STORAGE_PATH}/storage-{ac,cas}-0/persistent_state

docker-compose up "$@"
