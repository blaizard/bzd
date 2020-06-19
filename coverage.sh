#!/bin/bash
set -e

# Generate a coverage report

#./tools/bazel coverage --config=linux_x86_64_clang "$@"
./tools/bazel coverage "$@"
./tools/bazel run //tools/coverage:report -- --output-directory bazel-out/coverage bazel-out/_coverage/_coverage_report.dat
echo "Coverage report: $(pwd)/bazel-out/coverage/index.html"
