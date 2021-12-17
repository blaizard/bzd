#!/bin/bash

cd "/home/1000/sandbox"
./tools/bazel test ... --output_groups=+metadata --config=linux_x86_64_clang --config=dev --platform_suffix=_dev
