#!/bin/bash

set -e

rm -rfd "$(pwd)/docs/api"
bazel run //tools/documentation -- --output "$(pwd)/docs/api"
