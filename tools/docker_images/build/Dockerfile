FROM ubuntu:16.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
	sudo \
	ca-certificates \
	git

# GCC
COPY build_install_gcc.sh /build_install_gcc.sh
RUN /build_install_gcc.sh --install /usr/local 12.2.0

# Set environment variables
ENV PATH=/usr/local/bin:/bin:/usr/bin
ENV LD_LIBRARY_PATH=/usr/local/lib64:/usr/local/lib/x86_64-unknown-linux-gnu:/usr/lib64
ENV CC=/usr/local/bin/gcc
ENV CXX=/usr/local/bin/g++

# Python
COPY build_install_python.sh /build_install_python.sh
RUN /build_install_python.sh --install /usr/local 3.11.4
RUN ln -s /usr/local/bin/python3.11 /usr/bin/python3

# CMake
COPY build_install_cmake.sh /build_install_cmake.sh
RUN /build_install_cmake.sh 3.24.3

# Clang
COPY build_install_clang.sh /build_install_clang.sh
RUN /build_install_clang.sh 15.0.6

# Tools
RUN sudo apt-get install -y \
	patchelf
COPY patchall.py /usr/local/bin/patchall.py

WORKDIR /sandbox

ENTRYPOINT /bin/bash
