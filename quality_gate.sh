#!/bin/bash

set -e

# Handle script arguments
sanitizer=
while test $# -gt 0; do
    case "$1" in
    --sanitizer) sanitizer=1 && shift;;

    *) echo "Unrecognized argument \"$1\", abort." && exit 1 ;;
    esac
done

# Run sanitizer and check if any file changed, if so report an error
if [ "$sanitizer" ]; then
	expectedGitDiff=$(git diff --shortstat)
	./sanitize.sh
	if [ "$expectedGitDiff" != "$(git diff --shortstat)" ]; then
		echo "ERROR: Some files have been sanitized, please check and re-run." 1>&2
		git status 1>&2
		exit 1
	fi
fi

EXTRA_FLAGS="--sandbox_writable_path=$HOME/.cache/yarn --sandbox_writable_path=$HOME/.yarn"

# Compile and test the different configurations
./tools/bazel test ... --output_groups=default,metadata --config=dev --platform_suffix=_dev $EXTRA_FLAGS
./tools/bazel test ... --output_groups=default,metadata --config=prod --platform_suffix=_prod $EXTRA_FLAGS
./tools/bazel test ... --output_groups=default,metadata --config=linux_x86_64_clang $EXTRA_FLAGS
./tools/bazel build ... --output_groups=default,metadata --config=esp32_xtensa_lx6_gcc $EXTRA_FLAGS

# Use static analyzers
./tools/bazel test ... --config=linux_x86_64_clang --config=sanitizer --config=asan --config=lsan --platform_suffix=_clang_asan_lsan $EXTRA_FLAGS
