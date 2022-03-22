FROM ubuntu:20.04

USER root

ENV DEBIAN_FRONTEND=noninteractive

# Update the container
RUN apt-get update && apt-get upgrade -y

# Install packages
RUN apt-get install -y iptables openssh-server net-tools wget \
    && rm -rf /var/lib/apt/lists/*

# Set SSH
RUN mkdir -p /var/run/sshd
RUN mkdir -p ~/.ssh
COPY .ssh/config ~/.ssh/config
RUN sed -i '/GatewayPorts/c\GatewayPorts yes' /etc/ssh/sshd_config

EXPOSE 22
EXPOSE 1194

COPY executor.sh executor.sh
CMD ./executor.sh
