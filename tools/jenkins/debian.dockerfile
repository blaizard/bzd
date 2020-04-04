 
FROM debian:latest

# Load dependencies
RUN apt-get update && apt-get install -y git sudo g++ curl gnupg2

# Install Bazel
RUN echo "deb [arch=amd64] http://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list \
	&& curl https://bazel.build/bazel-release.pub.gpg | apt-key add -
RUN apt-get update && apt-get install -y bazel

# Add Jenkins user
RUN sudo groupadd -g 1000 1000
RUN sudo useradd -u 1000 -s /bin/sh -g 1000 1000
