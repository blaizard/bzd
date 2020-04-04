 
FROM debian:latest

# Load dependencies
RUN apt-get update && apt-get install -y git sudo g++ curl
RUN curl https://bazel.build/bazel-release.pub.gpg | sudo apt-key add -
RUN echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | sudo tee /etc/apt/sources.list.d/bazel.list
RUN apt-get update && apt-get install -y bazel

# Add Jenkins user
RUN sudo groupadd -g 1000 1000
RUN sudo useradd -u 1000 -s /bin/sh -g 1000 1000
