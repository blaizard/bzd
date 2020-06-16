#!/bin/bash
set -e

# Generate a coverage report

bazel coverage "$1"
genhtml --output-directory bazel-out/coverage bazel-out/_coverage/_coverage_report.dat
echo "Coverage report: $(pwd)/bazel-out/coverage/index.html"
