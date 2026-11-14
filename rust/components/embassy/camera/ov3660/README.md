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
memory = bzd.components.esp.memory.Psram(size = 8 * 1024 * 1024);
camera = bzd.components.embassy.camera.Ov3660(
    memory = memory,
    width = 2048,
    height = 1536,
    timeout_ms = 10000,
);
```

`memory` is required: use `Psram` for external PSRAM or `Heap` for the internal
`esp-alloc` heap. The frame buffer is `2 * width * height`, clamped to the
memory capacity.

`xclk_mhz` defaults to 20 MHz; try 10 MHz if the sensor does not stream
reliably. `white_balance` defaults to `true` and applies Espressif's Sunny
preset to avoid a green cast on some modules.

The hardware JPEG quality is automatic. The camera starts at the best quality
and, if a frame is truncated before its EOI (possible at QXGA when the readout
does not finish before the next frame), retries with a lower quality. On later
captures it first tries one step better. The working value is kept in RAM for
the component lifetime and resets on reboot or sleep.

Only the sizes listed in `settings.rs` are accepted, up to QXGA (2048×1536).

## Limits

- JPEG only, returned as `JpegFormat`.
- The DMA ring (8 KiB) stays in internal DRAM.
- The frame buffer comes from the configured `bzd.Memory`; large resolutions
  need PSRAM.
- `timeout_ms` bounds the whole capture, including quality retries.
- The returned frame is valid until the next `capture`.
- Cancelling a capture after the DMA starts drops the DMA ring; recreate the
  component in that case.
