FROM ubuntu:20.04

# Needed for dependencies which ask for some interactive input (besides yes and no)
ENV DEBIAN_FRONTEND=noninteractive

# Load dependencies
#
# Note:
# - nodejs needed for actions/checkout@v3
# - uidmap is needed for rootless docker
RUN apt-get update && apt-get install -y \
	git \
	curl \
	uidmap \
	sudo \
	build-essential \
	python3 \
	nodejs

# Install docker
RUN curl -sSL https://get.docker.com/ | sh

# Set user and group
ARG user=testuser
ARG group=testgroup
ARG uid=1000
ARG gid=1000

# Add Test user user
RUN sudo groupadd -g ${gid} ${group}
RUN sudo useradd -u ${uid} -s /bin/sh -g ${group} -m ${user}
RUN usermod -aG docker ${user}

# Switch to user
USER ${uid}:${gid}

# Install rootless docker (needs to be done as non-root)
RUN dockerd-rootless-setuptool.sh install
