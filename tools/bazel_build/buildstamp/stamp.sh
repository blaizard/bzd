#!/usr/bin/env bash

set -eu

REVISION=$(git rev-parse HEAD | tr -d '\n\r')
TIMESTAMP=$(git show --no-patch --format=%cd --date=format:%Y%m%d-%H%M%S ${REVISION})
NB_DIFFS=$(git status --porcelain 2>/dev/null | wc -l)
VERSION="${TIMESTAMP}-${REVISION}"
if [ "$NB_DIFFS" -ne "0" ]; then
    VERSION="${VERSION}-dirty"
fi

echo "STABLE_VERSION ${VERSION}"
