#!/bin/bash

set -e

# Sanitize
./tools/bazel run :sanitizer -- --all
#./tools/bazel run //tools/sanitizer:formatter
#./tools/bazel run //tools/sanitizer:analyzer
