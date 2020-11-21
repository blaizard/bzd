#!/bin/bash

set -e

EXTRA_FLAGS="--sandbox_writable_path=$HOME/.cache/yarn --sandbox_writable_path=$HOME/.yarn $@"

# Compile and test the different configurations
./tools/bazel test ... --output_groups=default,metadata --config=dev --platform_suffix=_dev $EXTRA_FLAGS
./tools/bazel test ... --output_groups=default,metadata --config=prod --platform_suffix=_prod $EXTRA_FLAGS
./tools/bazel test ... --output_groups=default,metadata --config=linux_x86_64_clang --config=cc $EXTRA_FLAGS
./tools/bazel build ... --output_groups=default,metadata --config=esp32_xtensa_lx6_gcc --config=cc $EXTRA_FLAGS

# Use static analyzers
./tools/bazel test ... --config=linux_x86_64_clang --config=cc --config=sanitizer --config=asan --config=lsan --platform_suffix=_clang_asan_lsan $EXTRA_FLAGS
