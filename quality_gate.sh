#!/bin/bash

set -e

bazel test ... --define=panic=throw
bazel test ... --define=panic=throw --config=linux_x86_64_clang --platform_suffix=_clang

bazel test ... --define=panic=throw --config=linux_x86_64_clang --config=asan --config=lsan --platform_suffix=_clang_asan_lsan

# Do sanatizing actions at the end
bazel run //tools/buildifier
