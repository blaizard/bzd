#!/bin/bash

set -e

bazel run //tools/buildifier

bazel test ... --copt="-std=c++14"
bazel test ... --config=linux_x86_64/clang --platform_suffix=_clang
bazel test ... --config=linux_x86_64/clang --config=asan --platform_suffix=_clang_asan
