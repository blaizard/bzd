 
FROM debian:latest

# Load dependencies
RUN apt-get update && apt-get install -y git sudo g++ curl

# Install Bazel
RUN curl -L https://github.com/bazelbuild/bazel/releases/download/2.2.0/bazel_2.2.0-linux-x86_64.deb -o /tmp/bazel.deb
RUN dpkg -i /tmp/bazel.deb

# Add Jenkins user
RUN sudo groupadd -g 1000 1000
RUN sudo useradd -u 1000 -s /bin/sh -g 1000 1000
