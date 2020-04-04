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

# Compile and test the different configurations
bazel test ... --sandbox_writable_path=~/.cache/yarn
bazel test ... --config=linux_x86_64_clang --platform_suffix=_linux_x86_64_clang --build_tag_filters="-nodejs"
bazel build ... --config=esp32_xtensa_lx6_gcc --platform_suffix=_esp32_xtensa_lx6_gcc --build_tag_filters="-nodejs"

# Use static analyzers
bazel test ... --config=linux_x86_64_clang --config=sanitizer --config=asan --config=lsan --platform_suffix=_clang_asan_lsan --build_tag_filters="-nodejs"
