FROM ubuntu:20.04

EXPOSE 10240

ARG DEBIAN_FRONTEND=noninteractive

# GCC

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    curl \
    build-essential \
    g++-multilib \
    xz-utils \
    libgmp-dev \
    libmpfr-dev \
    libmpc-dev \
    libisl-dev

COPY build_install_gcc.sh /build_install_gcc.sh
RUN /build_install_gcc.sh 10.2.0

# Clang

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    curl \
    ca-certificates \
    xz-utils \
    python3 \
    build-essential \
    cmake

COPY build_install_clang.sh /build_install_clang.sh
RUN /build_install_clang.sh 12.0.0

# Install node + compiler-explorer

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    npm \
    git \
    make \
    xz-utils \
    wget \
    python3

RUN wget https://nodejs.org/dist/v14.0.0/node-v14.0.0-linux-x64.tar.xz
RUN tar -C /usr/local --strip-components 1 -xJf node-v14.0.0-linux-x64.tar.xz
RUN git clone https://github.com/compiler-explorer/compiler-explorer.git
RUN cd compiler-explorer && make dist

WORKDIR /compiler-explorer

ENTRYPOINT ["/bin/bash", "-c"]
CMD ["make", "EXTRA_ARGS='--noCache --language c++'"]
