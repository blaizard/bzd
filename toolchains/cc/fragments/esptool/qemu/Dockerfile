FROM ubuntu:20.04

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

# Install dependencies for QEMU
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

# Install dependencies for GDB
RUN apt-get install -y --no-install-recommends \
    texinfo \
    bison \
    m4 \
    flex

# Install GDB, note this version already integrates the patch needed by previous version
# to avoid the "Remote 'g' packet reply is too long." error mesage.
# Follow: https://github.com/Ebiroll/qemu_esp32#building-gdb-81-for-qemu
RUN git clone --depth 1 --branch esp-2021r2-gdb https://github.com/espressif/binutils-gdb.git \
    && cd binutils-gdb \
	&& mkdir -p build \
	&& cd build \
	&& ../configure --without-guile --host=x86_64-pc-linux-gnu --target=xtensa-esp32-elf --disable-werror \
    && make \
    && make install

# Start from a Bash prompt
CMD ["/bin/bash"]
