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
COPY .ssh/config /root/.ssh/config
RUN sed -i '/GatewayPorts/c\GatewayPorts yes' /etc/ssh/sshd_config
RUN sed -i '/PubkeyAuthentication/c\PubkeyAuthentication yes' /etc/ssh/sshd_config
# Disallow password login
RUN sed -i '/PasswordAuthentication/c\PasswordAuthentication no' /etc/ssh/sshd_config
RUN sed -i '/PermitRootLogin/c\PermitRootLogin prohibit-password' /etc/ssh/sshd_config
RUN sed -i '/KbdInteractiveAuthentication/c\KbdInteractiveAuthentication no' /etc/ssh/sshd_config

EXPOSE 22
EXPOSE 1194

# Run the SSH daemon
ENTRYPOINT ["/usr/sbin/sshd", "-D"]
