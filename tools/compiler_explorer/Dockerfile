FROM ubuntu:20.04

EXPOSE 10240

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y software-properties-common

# GCC 10.2.0
RUN add-apt-repository ppa:ubuntu-toolchain-r/ppa
RUN apt-get update && apt-get install -y g++-10 clang-11 libc++-dev
RUN apt-get install -y npm git make xz-utils wget python3

# Install node
RUN wget https://nodejs.org/dist/v14.0.0/node-v14.0.0-linux-x64.tar.xz
RUN tar -C /usr/local --strip-components 1 -xJf node-v14.0.0-linux-x64.tar.xz

# Install compiler-explorer dependencies
RUN git clone https://github.com/compiler-explorer/compiler-explorer.git
RUN cd compiler-explorer && make dist

WORKDIR /compiler-explorer

ENTRYPOINT ["/bin/bash", "-c"]
CMD ["make", "EXTRA_ARGS='--noCache --language c++'"]
