# Partition table

The partition table used by the ESP32 and ESP32-S3 runners.

## File name

`partition_table_4mb_ota_3x1344kib.bin` describes the table:

- `4mb`: 4 MB flash.
- `ota`: factory plus two OTA slots (`ota_0`, `ota_1`).
- `3x1344kib`: three application slots of 1344 KiB (`0x150000`) each.

1344 KiB is `1.3125 MiB`, not 1.5 MiB. Three 1.5 MiB slots would exceed the
4 MB flash (`3 x 0x180000 = 0x480000`).

The binary is a 3072-byte (`0xC00`) table without the optional MD5 trailer,
matching the SDK bootloader configuration.

## Layout

| Name     | Type | SubType | Offset   | Size     |
| -------- | ---- | ------- | -------- | -------- |
| nvs      | data | nvs     | 0x9000   | 0x4000   |
| otadata  | data | ota     | 0xD000   | 0x2000   |
| phy_init | data | phy     | 0xF000   | 0x1000   |
| factory  | app  | factory | 0x10000  | 0x150000 |
| ota_0    | app  | ota_0   | 0x160000 | 0x150000 |
| ota_1    | app  | ota_1   | 0x2B0000 | 0x150000 |

The three application slots end exactly at `0x400000` (4 MB).

## Regenerating

`partitions.csv` documents the layout. After changing it, regenerate the binary
with Espressif's official `gen_esp32part.py` from ESP-IDF v5.4:

```bash
python3 gen_esp32part.py --disable-md5sum --flash-size 4MB \
    partitions.csv partition_table_4mb_ota_3x1344kib.bin
```

`--disable-md5sum` keeps the format used by the SDK bootloader.
