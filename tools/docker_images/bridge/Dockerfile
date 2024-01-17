FROM ubuntu:22.04

USER root

ENV DEBIAN_FRONTEND=noninteractive

# Update the container
RUN apt-get update && apt-get upgrade -y

# Install packages
RUN apt-get install -y openssh-server net-tools wget \
    && rm -rf /var/lib/apt/lists/*

# Set SSH
RUN mkdir -p /var/run/sshd
RUN mkdir -p ~/.ssh
COPY .ssh/config ~/.ssh/config
RUN sed -i '/GatewayPorts/c\GatewayPorts yes' /etc/ssh/sshd_config

EXPOSE 22
EXPOSE 1194

# Run the SSH daemon
ENTRYPOINT ["/usr/sbin/sshd", "-D"]
