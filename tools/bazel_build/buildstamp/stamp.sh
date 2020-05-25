#!/bin/bash
set -eu

REVISION=$(git rev-parse HEAD | tr -d '\n\r')
NB_DIFFS=$(git status --porcelain 2>/dev/null | wc -l)
if [ "$NB_DIFFS" -ne "0" ]; then
    REVISION="${REVISION}-dirty"
fi

echo "BZD_BUILD_REVISION ${REVISION}"
echo "BZD_BUILD_ISO8601 $(date --iso-8601=seconds --utc)"
echo "BZD_BUILD_TIMESTAMP $(date +%s --utc)"
