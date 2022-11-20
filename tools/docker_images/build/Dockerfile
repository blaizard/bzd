FROM debian/eol:wheezy

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
	sudo

# The ~/.bashrc file shall be used to add environment variables and aliases that will be
# loaded each time we use this image. This is used instead of ENV, to allow each bash scripts
# to add their own amendments to this file.
RUN echo "#!/bin/bash" > ~/.bashrc && chmod +x ~/.bashrc

# GCC 5 (needed for c++11 support to build other gcc).
COPY build_install_gcc.sh /build_install_gcc.sh
RUN /build_install_gcc.sh 5.5.0

# OpenSSL
COPY build_install_openssl.sh /build_install_openssl.sh
RUN /build_install_openssl.sh 1_0_2u

# GCC
#COPY build_install_gcc.sh /build_install_gcc.sh
#RUN /build_install_gcc.sh --static 12.2.0

# CMake
#COPY build_install_cmake.sh /build_install_cmake.sh
#RUN /build_install_cmake.sh 3.24.3

WORKDIR /sandbox

ENTRYPOINT /bin/bash
