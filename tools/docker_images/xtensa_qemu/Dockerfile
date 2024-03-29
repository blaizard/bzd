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

# Setup default tools
RUN git config --global user.email "dummy@domain.com" \
    && git config --global user.name "dummy"

# Bring patches
COPY 0001_fixing_number_of_registers_to_152.patch /bzd

# Install QEMU and apply a patch
RUN git clone --depth 1 https://github.com/espressif/qemu.git \
	&& cd qemu \
    && git am /bzd/0001_fixing_number_of_registers_to_152.patch \
	&& mkdir -p build \
	&& cd build \
	&& ../configure --target-list=xtensa-softmmu --enable-strip --disable-capstone --disable-vnc \
	&& make vga=no -j$(nproc) \
	&& make install

# Install dependencies for GDB
RUN apt-get install -y --no-install-recommends \
    texinfo \
    bison \
    m4 \
    flex \
    libncurses5 \
    libpython2.7 \
    pip

# Option 1 -----------------
# The following is to build GDB from source, however this doesn't work
# Install GDB, note this version already integrates the patch needed by previous version
# to avoid the "Remote 'g' packet reply is too long." error message.
# Follow: https://github.com/Ebiroll/qemu_esp32#building-gdb-81-for-qemu
#RUN git clone --depth 1 --branch esp-2021r2-gdb https://github.com/espressif/binutils-gdb.git

# Patch option 1
#RUN curl -L https://raw.githubusercontent.com/Ebiroll/gdb/master/gdb/xtensa-config.c.qemu --output binutils-gdb/gdb/xtensa-config.c

# Patch option 2
#RUN cd binutils-gdb \
#    && sed -i gdb/xtensa-config.c -e 's/\(XTREG([^,]\+,[^,]\+,[^,]\+,[^,]\+,[^,]\+,[^,]\+,[^,]\+\)/\1 \& ~1/'

#RUN cd binutils-gdb \
#	&& mkdir -p build \
#	&& cd build \
#	&& ../configure --without-guile --host=x86_64-pc-linux-gnu --target=xtensa-esp32-elf --disable-werror \
#   && make -j$(nproc) \
#   && make install

# Option 2 -----------------

# Use a prebuilt GDB
RUN curl -L https://github.com/espressif/crosstool-NG/releases/download/esp-2021r2/xtensa-esp32-elf-gcc8_4_0-esp-2021r2-linux-amd64.tar.gz --output /bzd/esp-2021r2.tar.gz \
    && tar -zxvf /bzd/esp-2021r2.tar.gz \
    && rm /bzd/esp-2021r2.tar.gz
ENV PATH="/bzd/xtensa-esp32-elf/bin:${PATH}"

# Install GDBGUI

RUN pip install gdbgui
EXPOSE 5000

# Start from a Bash prompt
CMD ["/bin/bash"]
