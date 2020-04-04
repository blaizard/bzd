 
FROM debian:latest

# Load dependencies
RUN apt-get update && apt-get install -y git sudo g++ bazel

# Add Jenkins user
RUN sudo groupadd -g 1000 1000
RUN sudo useradd -u 1000 -s /bin/sh -g 1000 1000
