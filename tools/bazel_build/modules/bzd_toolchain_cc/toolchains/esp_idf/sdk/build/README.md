# Build the esp32 bsp

1. Install ESP-IDF

```bash
# Install dependencies
sudo apt install \
	libusb-1.0-0 \
	python3.12-venv \
	cmake

git clone --recursive --branch "release/v5.4" https://github.com/espressif/esp-idf.git
./esp-idf/install.sh
. ./esp-idf/export.sh
```

2. Build

Run the build command.

```bash
bazel run @bzd_toolchain_cc//toolchains/esp_idf/sdk/build -- --output $(pwd)/dist/esp_idf/sdk --rebuild --env "$(env)" esp32
```

3. Copy the options

Do what the command line output says, and update the file `tools/bazel_build/modules/bzd_toolchain_cc/toolchains/esp_idf/sdk/defs.bzl`.

4. Create an archive

```bash
tar -cvJf esp32_xtensa_lx6_sdk.tar.xz -C dist/esp_idf/sdk esp32_xtensa_lx6_sdk
```
