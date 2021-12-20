FROM debian:stable

WORKDIR /bzd

ARG DEBIAN_FRONTEND=noninteractive

# Update the container
RUN apt-get update && apt-get upgrade -y

# Install essential dependencies
RUN apt-get install -y --no-install-recommends \
    git \
    cmake \
    curl \
    ca-certificates \
    xz-utils \
    build-essential

# Install specific dependencies
RUN apt-get install -y --no-install-recommends \
    ninja-build \
    python3 \
    pkg-config \
    libglib2.0-dev \
    libpixman-1-dev \
    libgcrypt20-dev

# Install QEMU
RUN git clone https://github.com/espressif/qemu.git \
	&& cd qemu \
	&& mkdir -p build \
	&& cd build \
	&& ../configure --target-list=xtensa-softmmu --enable-debug --enable-sanitizers --disable-strip --disable-capstone --disable-vnc \
	&& make vga=no \
	&& make install

# Start from a Bash prompt
CMD ["/bin/bash"]
