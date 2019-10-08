#!/bin/bash

set -e

bazel run //tools/buildifier

bazel test ... --copt="-std=c++14"
bazel test ... --config=linux_x86_64/clang
