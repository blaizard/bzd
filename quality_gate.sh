#!/bin/bash

set -e

bazel run //tools/buildifier

bazel test ... --define=panic=throw --copt="-std=c++14"
bazel test ... --define=panic=throw --config=linux_x86_64_clang --platform_suffix=_clang
bazel test ... --define=panic=throw --config=linux_x86_64_clang --config=asan --config=lsan --platform_suffix=_clang_asan_lsan
