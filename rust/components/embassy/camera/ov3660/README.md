# OV3660 camera

OV3660 DVP camera component for the Seeed Studio XIAO ESP32-S3 Sense,
implementing `bzd.Camera` through esp-hal's LCD_CAM peripheral.

The OV3660 is a fixed-focus sensor, so there is no autofocus control.

## Wiring

The Sense expansion board uses:

| Signal      | GPIO |
| ----------- | ---- |
| XCLK        | 10   |
| DVP Y2 (D0) | 15   |
| DVP Y3 (D1) | 17   |
| DVP Y4 (D2) | 18   |
| DVP Y5 (D3) | 16   |
| DVP Y6 (D4) | 14   |
| DVP Y7 (D5) | 12   |
| DVP Y8 (D6) | 11   |
| DVP Y9 (D7) | 48   |
| PCLK        | 13   |
| VSYNC       | 38   |
| HREF        | 47   |
| SCCB SDA    | 40   |
| SCCB SCL    | 39   |

PWDN and RESET are not wired on this board.

## Configuration

```bdl
camera = bzd.components.embassy.camera.Ov3660(
    width = 640,
    height = 480,
    quality = 12,
    timeout_ms = 5000,
    xclk_mhz = 20,
    white_balance = true,
);
```

Only the frame sizes listed in `settings.rs` are accepted, up to QXGA
(2048×1536). The default VGA frame fits the internal-DRAM buffers used by this
component.

`xclk_mhz` defaults to 20 MHz. If the sensor does not stream reliably, try
10 MHz, which is the value used by the `esp32s3-cam-async` OV3660 example.

`white_balance` applies Espressif's manual Sunny white-balance preset, which
avoids a green cast observed on some OV3660 modules. Set it to `false` to keep
the sensor's automatic white balance.

## Limits

- JPEG output only, returned as `JpegFormat`.
- The DMA ring and frame buffer are 64 KiB each and live in internal DRAM.
  Larger resolutions need larger constants or a PSRAM-backed buffer.
- The returned frame is valid until the next `capture`.
- A `capture` future that is cancelled after the DMA transfer starts drops the
  DMA ring; recreate the component in that case.
