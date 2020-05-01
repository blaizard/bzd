FROM ubuntu:latest

# Load dependencies
# Note:
# - python - required to run python files, eventhough a python toolchain is installed,
#            to run a script using this python toolchain, a startup python script needs
#            to be executed to discover binaries.
# - gnupg2 - needed add the key to apt.
# - libxml2-dev - needed for LLVM (need to be removed).
# - openjdk-11-jdk, unzip, zip are used by bazel
RUN apt-get update && apt-get install -y git sudo build-essential g++ unzip zip libc6-dev curl gnupg2 python libxml2-dev openjdk-11-jdk

# Install Bazel
RUN echo "deb [arch=amd64] http://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list \
	&& curl https://bazel.build/bazel-release.pub.gpg | apt-key add -
RUN apt-get update && apt-get install -y bazel

# Add Jenkins user
RUN sudo groupadd -g 1000 1000
RUN sudo useradd -u 1000 -s /bin/sh -g 1000 1000
RUN sudo mkdir -p /home/1000
RUN sudo chown -R 1000:1000 /home/1000
