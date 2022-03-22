#!/bin/bash

# Copy the key to the authorized keys
wget "https://raw.githubusercontent.com/blaizard/cpp-async/master/tools/docker_images/bridge/nas.pub" -O ~/.ssh/authorized_keys

# Run the SSH daemon
echo "Starting the SSH daemon"
/usr/sbin/sshd -D
