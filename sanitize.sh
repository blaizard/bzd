#!/bin/bash

set -e

# Sanitize
bazel run //tools/sanitizer -- --workspace $(pwd) --action clang-format
bazel run //tools/buildifier

# Generate documentation
rm -rfd "$(pwd)/docs/api"
bazel run //tools/documentation -- --output "$(pwd)/docs/api"
