#!/bin/bash

apt-get purge -y bazel

url="https://github.com/bazelbuild/bazel/releases/download/2.0.0/bazel_2.0.0-linux-x86_64.deb"

curl -L ${url} -o ./tmp.bazel.build
dpkg -i ./tmp.bazel.build
rm -f ./tmp.bazel.build
