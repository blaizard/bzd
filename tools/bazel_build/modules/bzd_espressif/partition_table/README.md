# Partition table

The binaries are generated from the `partitions*.csv` files with Espressif's
official `gen_esp32part.py` from ESP-IDF v5.4:

```bash
python3 gen_esp32part.py --disable-md5sum --flash-size 8MB \
    partitions_8mb_ota_2x4032kib.csv partition_table_8mb_ota_2x4032kib.bin
```

`--disable-md5sum` keeps the format used by the SDK bootloader.
