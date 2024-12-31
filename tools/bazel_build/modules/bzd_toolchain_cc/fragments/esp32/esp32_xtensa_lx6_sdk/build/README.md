# Build the esp32 bsp

1. Install ESP-IDF

```bash
git clone --recursive https://github.com/espressif/esp-idf.git
./esp-idf/install.sh esp32
. ./esp-idf/export.sh
```

2. Build

Run the build command.

```bash
bazel run @bzd_toolchain_cc//fragments/esp32/esp32_xtensa_lx6_sdk/build -- --output $(pwd)/dist/esp32_xtensa_lx6_sdk --rebuild --env "$(env)"
```

3. Copy the options

Do what the command line output says, and update the file `tools/bazel_build/modules/bzd_toolchain_cc/fragments/esp32/esp32_xtensa_lx6_sdk/defs.bzl`.
