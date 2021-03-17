FROM ubuntu:bionic
# Note updating to ubuntu:20 brings this error message with python:
# ImportError: libffi.so.6: cannot open shared object file: No such file or directory

# Needed for dependencies like "tzdata" which might ask for some interactive input (besides yes and no)
ENV DEBIAN_FRONTEND=noninteractive

# Load dependencies
# Note:
# - python - required to run python files, eventhough a python toolchain is installed,
#            to run a script using this python toolchain, a startup python script needs
#            to be executed to discover binaries.
# - gpgv - needed add the key to apt.
# - libxml2-dev - needed for LLVM (need to be removed).
# - openjdk-11-jdk, unzip, zip are used by bazel
RUN apt-get update && apt-get install -y git \
	sudo \
	build-essential \
	unzip \
	zip \
	gzip \
	libc6-dev \
	curl \
	gpgv \
	gnupg \
	python \
	libxml2-dev \
	openjdk-11-jdk \
	apt-transport-https \
	ca-certificates \
	lsb-release

# Add Docker's official GPG key
RUN curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /usr/share/keyrings/docker-archive-keyring.gpg

# Set up the stable docker repository
RUN echo \
	"deb [arch=amd64 signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] https://download.docker.com/linux/ubuntu \
	$(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

# Install docker engine
RUN apt-get update && apt-get install -y docker-ce \
	docker-ce-cli \
	containerd.io

# Add Jenkins user
RUN sudo groupadd -g 1000 1000
RUN sudo useradd -u 1000 -s /bin/sh -g 1000 1000
RUN sudo mkdir -p /home/1000
RUN sudo chown -R 1000:1000 /home/1000

RUN usermod -aG docker 1000

ENV DOCKER_HOST=tcp://host:2375
