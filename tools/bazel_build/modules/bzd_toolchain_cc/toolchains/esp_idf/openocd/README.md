# Openocd

## ESPLink

https://github.com/wuxx/ESPLink

Using the esp32s3 as a target chip.

```bash
sudo chmod -R 777 /dev/bus/usb/
bazel run @esp32_openocd//:openocd -- -f interface/esp_usb_bridge.cfg -f target/esp32s3.cfg
```

or

```bash
NIXPKGS_ALLOW_UNFREE=1 nix-shell -p steam-run --run "steam-run ./bin/openocd -s ./share/openocd/scripts -f interface/esp_usb_bridge.cfg -f target/esp32s3.cfg"
```

Debug with gdb

```bash
xtensa-esp32s3-elf-gdb -ex 'target remote 127.0.0.1:3333' ./build/blink.elf
```

```bash
NIXPKGS_ALLOW_UNFREE=1 nix-shell -p steam-run --run "steam-run ./bin/openocd -s ./share/openocd/scripts -f interface/esp_usb_bridge.cfg -f target/esp32s3.cfg"
```

```bash
cat << EOF | sudo tee /etc/udev/rules.d/70-esp32s3-jtag.rules
ATTRS{idVendor}=="303a", ATTRS{idProduct}=="1002", MODE="0660", GROUP="plugdev", TAG+="uaccess"
EOF
sudo udevadm control --reload-rules && sudo udevadm trigger
```
