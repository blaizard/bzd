#!/bin/bash

set -e

EXTRA_FLAGS="--sandbox_writable_path=$HOME/.cache/yarn --sandbox_writable_path=$HOME/.yarn $@"

# Compile and test the different configurations
./tools/bazel test ...  $EXTRA_FLAGS --output_groups=default,metadata --config=dev --platform_suffix=_dev
./tools/bazel test ...  $EXTRA_FLAGS --output_groups=default,metadata --config=prod --platform_suffix=_prod
./tools/bazel test ...  $EXTRA_FLAGS --output_groups=default,metadata --config=linux_x86_64_clang --config=cc
./tools/bazel test ...  $EXTRA_FLAGS --output_groups=default,metadata --config=linux_x86_64_gcc --config=cc
./tools/bazel build ...  $EXTRA_FLAGS --output_groups=default,metadata --config=esp32_xtensa_lx6_gcc --config=cc

# Stress tests
./tools/bazel test ... $EXTRA_FLAGS --build_tag_filters=stress --test_tag_filters=stress --config=dev --runs_per_test=100 --platform_suffix=_dev
./tools/bazel test ... $EXTRA_FLAGS --build_tag_filters=stress --test_tag_filters=stress --config=prod --runs_per_test=100 --platform_suffix=_prod
./tools/bazel test ... $EXTRA_FLAGS --build_tag_filters=stress --test_tag_filters=stress --runs_per_test=100 -config=linux_x86_64_clang

# Use static analyzers
./tools/bazel test ...  $EXTRA_FLAGS --config=linux_x86_64_clang --config=cc --config=sanitizer --config=asan --config=lsan --platform_suffix=_clang_asan_lsan
