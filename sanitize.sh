#!/bin/bash

set -e

WORKSPACE_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Sanitize
./tools/bazel run //tools/sanitizer:formatter
./tools/bazel run //tools/sanitizer:analyzer
