#!/bin/bash

set -e

EXTRA_FLAGS="$@"

echo  "==== normal linux_x86_64_clang dev + clang-tidy ==============================="
./tools/bazel test ... --output_groups=+metadata --config=linux_x86_64_clang --config=dev --config=clang_tidy --platform_suffix=_linux_x86_64_clang_dev $EXTRA_FLAGS
echo  "==== normal linux_x86_64_clang prod ==============================="
./tools/bazel test ... --output_groups=+metadata --config=linux_x86_64_clang --config=prod --platform_suffix=_linux_x86_64_clang_prod $EXTRA_FLAGS
echo  "==== normal linux_x86_64_gcc ==============================="
./tools/bazel test ... --output_groups=+metadata --config=linux_x86_64_gcc --config=prod --config=cc --platform_suffix=_linux_x86_64_gcc_prod $EXTRA_FLAGS
echo  "==== normal esp32_xtensa_lx6_gcc ==============================="
./tools/bazel test ... --output_groups=+metadata --config=esp32_xtensa_lx6_gcc --config=prod --config=cc --platform_suffix=_esp32_xtensa_lx6_gcc_prod $EXTRA_FLAGS
echo  "==== stress dev ==============================="
./tools/bazel test ... --build_tag_filters=stress --test_tag_filters=stress --config=linux_x86_64_clang --config=dev --runs_per_test=100 --platform_suffix=_linux_x86_64_clang_dev $EXTRA_FLAGS
echo  "==== stress prod ==============================="
./tools/bazel test ... --build_tag_filters=stress --test_tag_filters=stress --config=linux_x86_64_clang --config=prod --runs_per_test=100 --platform_suffix=_linux_x86_64_clang_prod $EXTRA_FLAGS
echo  "==== sanitizer asan/lsan ==============================="
./tools/bazel test ... --config=linux_x86_64_clang --config=cc --config=sanitizer --config=asan --config=lsan --platform_suffix=_clang_asan_lsan $EXTRA_FLAGS
echo  "==== coverage C++ ==============================="
./tools/bazel coverage cc/... --config=linux_x86_64_gcc --config=cc --platform_suffix=_coverage_cc && ./tools/bazel run tools/coverage -- --output bazel-out/coverage_cc $EXTRA_FLAGS
echo  "==== coverage Python ==============================="
./tools/bazel coverage ... --config=py --platform_suffix=_coverage_py && ./tools/bazel run tools/coverage -- --output bazel-out/coverage_py $EXTRA_FLAGS
echo  "==== coverage NodeJs ==============================="
./tools/bazel coverage ... --config=nodejs --platform_suffix=_coverage_nodejs && ./tools/bazel run tools/coverage -- --output bazel-out/coverage_nodejs $EXTRA_FLAGS
echo  "==== sanitizer sanitizer ==============================="
./sanitize.sh $EXTRA_FLAGS
