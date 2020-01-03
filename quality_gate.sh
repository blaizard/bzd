#!/bin/bash

set -e

# Do sanitizing actions at the beginnig
bazel run //tools/sanitizer -- --workspace $(pwd) --action clang-format
bazel run //tools/buildifier

# Compile and test the different configurations
bazel test ... --define=panic=throw
bazel test ... --define=panic=throw --config=linux_x86_64_clang --platform_suffix=_clang

# Use static analyzers
bazel test ... --define=panic=throw --config=linux_x86_64_clang --config=asan --config=lsan --platform_suffix=_clang_asan_lsan
