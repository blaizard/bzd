#!/bin/bash

set -e

bazel test ... --config=linux_x86_64/gcc
bazel test ... --config=linux_x86_64/clang
