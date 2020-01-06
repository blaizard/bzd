#!/bin/bash

apt-get purge -y bazel

url="https://github.com/bazelbuild/bazel/releases/download/2.0.0/bazel_2.0.0-linux-x86_64.deb"

mkdir -p ./tmp.bazel.build
curl -L ${url} -o ./tmp.bazel.build/${package}
dpkg -i ./tmp.bazel.build/${package}
rm -rfd ./tmp.bazel.build
