FROM ubuntu:24.04

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
	nodejs \
	jq \
	sed \
	openssh-client

# Set user and group.
ARG user=testuser
ARG group=testgroup
ARG uid=1000
ARG gid=1000

# Add test user	and group.
RUN id -u ${uid} &>/dev/null && userdel -r $(id -un ${uid}) || true
RUN getent group ${gid} &>/dev/null && groupdel $(getent group ${gid} | cut -d: -f1) || true
RUN groupadd -g ${gid} ${group}
RUN useradd -s /bin/sh --uid ${uid} --gid ${gid} -m ${user} --groups sudo
RUN echo "\"${user}\" ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

# Switch to user
USER ${uid}:${gid}

# Add known hosts for git operations
RUN mkdir -p ~/.ssh
RUN ssh-keyscan github.com >> ~/.ssh/known_hosts
RUN ssh-keyscan -p 6222 10.10.0.2 >> ~/.ssh/known_hosts
RUN chmod 644 ~/.ssh/known_hosts
