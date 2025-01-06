#!/bin/bash

# Do not modify, this file has been generated by //tools/ci

set -e

EXTRA_FLAGS="$@"

echo  "==== [normal] static analysis ==============================="
./tools/bazel test --target_pattern_file=tools/ci/bazel_target_patterns.txt --output_groups=+metadata --config=dev --config=clang-tidy --config=mypy --platform_suffix=static_analysis $EXTRA_FLAGS
echo  "==== [normal] clang prod ==============================="
./tools/bazel test --target_pattern_file=tools/ci/bazel_target_patterns.txt --output_groups=+metadata --config=clang --config=prod --config=cc --platform_suffix=clang-prod $EXTRA_FLAGS
echo  "==== [normal] gcc prod ==============================="
./tools/bazel test --target_pattern_file=tools/ci/bazel_target_patterns.txt --output_groups=+metadata --config=gcc --config=prod --config=cc --platform_suffix=gcc-prod $EXTRA_FLAGS
echo  "==== [normal] esp32 prod ==============================="
./tools/bazel test --target_pattern_file=tools/ci/bazel_target_patterns.txt --output_groups=+metadata --config=esp32 --config=prod --config=cc --platform_suffix=esp32-prod $EXTRA_FLAGS
echo  "==== [normal] esp32s3 prod ==============================="
./tools/bazel test --target_pattern_file=tools/ci/bazel_target_patterns.txt --output_groups=+metadata --config=esp32s3 --config=prod --config=cc --platform_suffix=esp32s3-prod $EXTRA_FLAGS
echo  "==== [stress] dev (100 runs) ==============================="
./tools/bazel test --target_pattern_file=tools/ci/bazel_target_patterns.txt --build_tests_only --test_tag_filters=stress,-cc-coverage --config=dev --runs_per_test=100 --local_test_jobs=1 --platform_suffix=stress-dev $EXTRA_FLAGS
echo  "==== [stress] prod (100 runs) ==============================="
./tools/bazel test --target_pattern_file=tools/ci/bazel_target_patterns.txt --build_tests_only --test_tag_filters=stress,-cc-coverage --config=prod --runs_per_test=100 --local_test_jobs=1 --platform_suffix=stress-prod $EXTRA_FLAGS
echo  "==== [sanitizer] asan/lsan/ubsan ==============================="
./tools/bazel test --target_pattern_file=tools/ci/bazel_target_patterns.txt --config=clang --config=cc --config=sanitizer --config=asan --config=lsan --config=ubsan --platform_suffix=clang-asan-lsan-ubsan $EXTRA_FLAGS
echo  "==== [sanitizer] tsan ==============================="
./tools/bazel test --target_pattern_file=tools/ci/bazel_target_patterns.txt --config=clang --config=cc --config=sanitizer --config=tsan --platform_suffix=clang-tsan $EXTRA_FLAGS
echo  "==== [coverage] cc ==============================="
./tools/bazel coverage cc/... --config=gcc --config=cc --platform_suffix=coverage-cc --config=local $EXTRA_FLAGS
./tools/bazel run @bzd_coverage//:coverage --platform_suffix=coverage-cc --config=local -- --output bazel-out/coverage-cc $EXTRA_FLAGS
echo  "==== [coverage] nodejs ==============================="
./tools/bazel coverage ... --config=nodejs --platform_suffix=coverage-nodejs --config=local $EXTRA_FLAGS
./tools/bazel run @bzd_coverage//:coverage --platform_suffix=coverage-nodejs --config=local -- --output bazel-out/coverage-nodejs $EXTRA_FLAGS
echo  "==== [sanitizer] sanitizer ==============================="
./tools/bazel run :sanitizer -- --check --all $EXTRA_FLAGS
