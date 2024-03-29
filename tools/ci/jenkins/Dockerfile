FROM ubuntu:20.04

# Needed for dependencies like "tzdata" which might ask for some interactive input (besides yes and no)
ENV DEBIAN_FRONTEND=noninteractive

# Load dependencies
# Note:
# - python - required to run python files, even though a python toolchain is installed,
#            to run a script using this python toolchain, a startup python script needs
#            to be executed to discover binaries.
# - gpgv - needed add the key to apt.
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
	python3 \
	python3-distutils \
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
RUN sudo groupadd -g 1000 jenkins
RUN sudo useradd -u 1000 -s /bin/sh -g jenkins jenkins
RUN sudo mkdir -p /home/jenkins
RUN sudo chown -R jenkins:jenkins /home/jenkins

# Add the user to the docker group
RUN usermod -aG docker jenkins
