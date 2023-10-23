#!/bin/bash

# Do not modify, this file has been generated by //tools/ci

set -e

EXTRA_FLAGS="$@"

echo  "==== [normal] python ==============================="
./tools/bazel test @bzd_python//... --output_groups=+metadata --config=prod --platform_suffix=python $EXTRA_FLAGS
echo  "==== [normal] clang-tidy ==============================="
./tools/bazel test ... --output_groups=+metadata --config=dev --config=clang-tidy --platform_suffix=clang-tidy $EXTRA_FLAGS
echo  "==== [normal] clang prod ==============================="
./tools/bazel test ... --output_groups=+metadata --config=clang --config=prod --config=cc --platform_suffix=clang-prod $EXTRA_FLAGS
echo  "==== [normal] gcc prod ==============================="
./tools/bazel test ... --output_groups=+metadata --config=gcc --config=prod --config=cc --platform_suffix=gcc-prod $EXTRA_FLAGS
echo  "==== [normal] esp32 prod ==============================="
./tools/bazel test ... --output_groups=+metadata --config=esp32 --config=prod --config=cc --platform_suffix=esp32-prod $EXTRA_FLAGS
echo  "==== [stress] dev (100 runs) ==============================="
./tools/bazel test ... --build_tests_only --test_tag_filters=stress,-cc-coverage --config=dev --runs_per_test=100 --platform_suffix=stress-dev $EXTRA_FLAGS
echo  "==== [stress] prod (100 runs) ==============================="
./tools/bazel test ... --build_tests_only --test_tag_filters=stress,-cc-coverage --config=prod --runs_per_test=100 --platform_suffix=stress-prod $EXTRA_FLAGS
echo  "==== [sanitizer] asan/lsan/ubsan ==============================="
./tools/bazel test ... --config=clang --config=cc --config=sanitizer --config=asan --config=lsan --config=ubsan --platform_suffix=clang-asan-lsan-ubsan $EXTRA_FLAGS
echo  "==== [sanitizer] tsan ==============================="
./tools/bazel test ... --config=clang --config=cc --config=sanitizer --config=tsan --platform_suffix=clang-tsan $EXTRA_FLAGS
echo  "==== [coverage] C++ ==============================="
./tools/bazel coverage cc/... --config=gcc --config=cc --platform_suffix=coverage-cc && ./tools/bazel run tools/coverage --platform_suffix=coverage-cc -- --output bazel-out/coverage-cc $EXTRA_FLAGS
echo  "==== [coverage] NodeJs ==============================="
./tools/bazel coverage ... --config=nodejs --platform_suffix=coverage-nodejs && ./tools/bazel run tools/coverage --platform_suffix=coverage-nodejs -- --output bazel-out/coverage-nodejs $EXTRA_FLAGS
echo  "==== [sanitizer] sanitizer ==============================="
./tools/bazel run :sanitizer -- --check --all $EXTRA_FLAGS
