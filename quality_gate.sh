#!/bin/bash

set -e

EXTRA_FLAGS="$@"

# Compile and test the different configurations
echo  "==== linux_x86_64_clang dev + clang-tidy ==============================="
./tools/bazel test ...  $EXTRA_FLAGS --output_groups=+metadata --config=linux_x86_64_clang --config=dev --config=clang_tidy --platform_suffix=_dev
echo  "==== linux_x86_64_clang prod ==========================================="
./tools/bazel test ...  $EXTRA_FLAGS --output_groups=+metadata --config=linux_x86_64_clang --config=prod --platform_suffix=_prod
echo  "==== linux_x86_64_gcc =================================================="
./tools/bazel test ...  $EXTRA_FLAGS --output_groups=+metadata --config=linux_x86_64_gcc --config=cc
echo  "==== esp32_xtensa_lx6_gcc =============================================="
./tools/bazel build ...  $EXTRA_FLAGS --output_groups=+metadata --config=esp32_xtensa_lx6_gcc --config=cc

# Stress tests
echo  "==== stress dev ========================================================"
./tools/bazel test ... $EXTRA_FLAGS --build_tag_filters=stress --test_tag_filters=stress --config=linux_x86_64_clang --config=dev --runs_per_test=100 --platform_suffix=_dev
echo  "==== stress prod ======================================================="
./tools/bazel test ... $EXTRA_FLAGS --build_tag_filters=stress --test_tag_filters=stress --config=linux_x86_64_clang --config=prod --runs_per_test=100 --platform_suffix=_prod

# Use static analyzers
echo  "==== sanitizer asan/lsan ==============================================="
./tools/bazel test ...  $EXTRA_FLAGS --config=linux_x86_64_clang --config=cc --config=sanitizer --config=asan --config=lsan --platform_suffix=_clang_asan_lsan
