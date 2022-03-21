FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive

# Update the container
RUN apt-get update && apt-get upgrade -y

# Install packages
RUN apt-get install -y openssh-server net-tools

# socat TCP-LISTEN:443,fork TCP:127.0.0.1:5000

EXPOSE 22
EXPOSE 443
