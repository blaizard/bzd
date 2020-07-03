#!/bin/bash

set -e

WORKSPACE_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Sanitize
./tools/bazel run //tools/sanitizer
./tools/bazel run //tools/buildifier -- -lint "fix" -r "$WORKSPACE_PATH"

# Generate documentation
rm -rfd "$WORKSPACE_PATH/docs/api"
mkdir "$WORKSPACE_PATH/docs/api" "$WORKSPACE_PATH/docs/api/cc" "$WORKSPACE_PATH/docs/api/bazel"

./tools/bazel run //tools/documentation/cc -- --output "$WORKSPACE_PATH/docs/api/cc"
./tools/bazel run //tools/documentation/bazel -- "$WORKSPACE_PATH/docs/api/bazel"
