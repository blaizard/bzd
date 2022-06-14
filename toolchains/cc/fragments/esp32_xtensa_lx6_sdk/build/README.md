# Build the esp32 bsp

1. Install ESP-IDF

```bash
git clone --recursive https://github.com/espressif/esp-idf.git
./esp-idf/install.sh esp32
. ./esp-idf/export.sh
```

2. Go to the project directory and compile

```bash
cd "toolchains/cc/fragments/esp32_xtensa_lx6_sdk/build/project"
idf.py clean build
```
