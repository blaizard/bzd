#!/bin/bash

set -e

# Run sanitizer and check if any file changed, if so report an error
expectedGitDiff=$(git diff --shortstat)
./sanitize.sh
if [ "$expectedGitDiff" != "$(git diff --shortstat)" ]; then
	echo "ERROR: Some files have been sanitized, please check and re-run."
	exit 1
fi

# Compile and test the different configurations
bazel test ... --define=panic=throw
bazel test ... --define=panic=throw --config=linux_x86_64_clang --platform_suffix=_clang

# Use static analyzers
bazel test ... --define=panic=throw --config=linux_x86_64_clang --config=asan --config=lsan --platform_suffix=_clang_asan_lsan
