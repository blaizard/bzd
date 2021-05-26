FROM ubuntu:20.04

EXPOSE 10240

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update

# GCC 10.2.0
RUN apt-get install -y g++-10=10.2.0-5ubuntu1~20.04
RUN apt-get install -y npm git make xz-utils wget python3

# Install node
RUN wget https://nodejs.org/dist/v12.13.1/node-v12.13.1-linux-x64.tar.xz
RUN tar -C /usr/local --strip-components 1 -xJf node-v12.13.1-linux-x64.tar.xz

# Install compiler-explorer dependencies
RUN git clone https://github.com/compiler-explorer/compiler-explorer.git
RUN cd compiler-explorer && make dist

WORKDIR /compiler-explorer

ENTRYPOINT ["/bin/bash", "-c"]
CMD ["make", "EXTRA_ARGS='--language c++'"]
