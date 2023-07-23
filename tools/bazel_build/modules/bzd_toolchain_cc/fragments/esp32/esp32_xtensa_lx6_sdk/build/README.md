# Build the esp32 bsp

1. Install ESP-IDF

```bash
git clone --recursive https://github.com/espressif/esp-idf.git
./esp-idf/install.sh esp32
. ./esp-idf/export.sh
```

2. Build

Run the build command with the optional output argument.

```bash
rm -f toolchains/cc/fragments/esp32_xtensa_lx6_sdk/build/project/sdkconfig
bazel run toolchains/cc/fragments/esp32_xtensa_lx6_sdk/build -- --output ~/sandbox/bzd-esp32-xtensa-sdk/
```

3. Copy the options

Do what the command line output says, and update the file `toolchains/cc/fragments/esp32_xtensa_lx6_sdk/defs.bzl`.
