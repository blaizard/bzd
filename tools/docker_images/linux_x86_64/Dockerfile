FROM ubuntu:20.04

# Needed for dependencies like "tzdata" which might ask for some interactive input (besides yes and no)
ENV DEBIAN_FRONTEND=noninteractive

# Load dependencies
# Note:
# - python - required to run python files, even though a python toolchain is installed,
#            to run a script using this python toolchain, a startup python script needs
#            to be executed to discover binaries.
# - openjdk-11-jdk, unzip, zip are used by bazel
RUN apt-get update && apt-get install -y git \
	sudo \
	build-essential \
	unzip \
	zip \
	gzip \
	libc6-dev \
	python \
	python3 \
	python3-distutils \
	openjdk-11-jdk \
	apt-transport-https \
	ca-certificates \
	lsb-release
