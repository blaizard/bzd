FROM ubuntu:20.04

WORKDIR /bzd

ARG DEBIAN_FRONTEND=noninteractive

# Update the container
RUN apt-get update && apt-get upgrade -y

# Install dependencies for QEMU / GDB
RUN apt-get install -y --no-install-recommends \
    git \
    wget \
    flex \
    bison \
    gperf \
    build-essential \
    cmake \
    ninja-build \
    ccache \
    python3 \
    python3-pip \
    python3-venv \
    pkg-config \
    libglib2.0-dev \
    libpixman-1-dev \
    libgcrypt20-dev \
    libffi-dev \
    libssl-dev \
    dfu-util \
    libusb-1.0-0 \
    libslirp-dev

# Install QEMU
RUN git clone --quiet https://github.com/espressif/qemu.git \
    && cd qemu \
	&& mkdir -p build \
	&& cd build \
	&& ../configure --target-list=xtensa-softmmu --without-default-features --enable-slirp  --enable-gcrypt \
	&& make -j $(nproc) vga=no \
	&& make install \
    rm -rfd qemu

# Install GDB
RUN wget https://github.com/espressif/binutils-gdb/releases/download/esp-gdb-v12.1_20221002/xtensa-esp-elf-gdb-12.1_20221002-x86_64-linux-gnu.tar.gz -O /bzd/gdb.tar.gz \
    && tar -zxvf /bzd/gdb.tar.gz \
    && rm /bzd/gdb.tar.gz
ENV PATH="/bzd/xtensa-esp32-elf/bin:${PATH}"

# Install GDBGUI
RUN pip install gdbgui
EXPOSE 5000

# Start from a Bash prompt
CMD ["/bin/bash"]
