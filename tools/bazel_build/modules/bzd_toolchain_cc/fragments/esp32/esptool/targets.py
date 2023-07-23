import pathlib

current_directory = pathlib.Path(__file__).parent
targets = {
    "esp32": {
        "usbIds": [
            # Silicon Labs CP210x devices (USB/UART bridge)
            (0x10c4, 0xea60)
        ],
        "memorySize":
            4 * 1024 * 1024,
        "memoryMap": {
            0x1000: f"{current_directory}/bin/esp32/bootloader.bin",
            0x8000: f"{current_directory}/bin/esp32/partitions_singleapp.bin",
            0x10000: "{binary}"
        },
        "args":
            "--chip esp32 --port {device} --baud 460800 --before default_reset --after hard_reset write_flash --flash_mode dio --flash_freq 40m --flash_size detect {memory}"
    }
}
