#!/bin/bash

set -e

# Compile and test the different configurations
bazel test ... --define=panic=throw
bazel test ... --define=panic=throw --config=linux_x86_64_clang --platform_suffix=_clang

# Use static analyzers
bazel test ... --define=panic=throw --config=linux_x86_64_clang --config=asan --config=lsan --platform_suffix=_clang_asan_lsan
