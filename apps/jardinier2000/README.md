# Jardinier2000

## Software

::: //apps/jardinier2000:diagram

### Program

```bash
./tools/bazel run //apps/jardinier2000:jardinier2000.platform --config=esp32s3 --run_under=@bzd_espressif//runner/esp32s3:uart --config=prod
```

## Hardware

- v0.1: a6dc53e17e0d28d296f038ecd4dc024c1fd0ac4c
- v0.2: 74176779d8ccca15caded8594085d71fdf246d6d
- v0.3: (wip)
  - Step down:
    - Updated coil 3.3uH 6x6mm -> 4.7uH 8x8mm.
    - Adding C21 & C22 at VIN to damp voltage spike.
  - Moved SW1 from IO45 -> IO47 (IO45 is a special pin).
  - Reset capacitor (C6) 0.1uF -> 1uF as recommended by Espressif.

TODO: 3.3V LDO Dropout Voltage Margin (U2 - NCP1117-3.3)

- Issue: The 3.3V rail uses an NCP1117-3.3 LDO (U2), which has a high dropout voltage ($V_{DO} \approx 1.0\text{V} - 1.2\text{V}$). The LDO input is supplied from 5V through Schottky diodes D1 or D7 ($\Delta V \approx 0.4\text{V}$), leaving $V_{IN} \approx 4.6\text{V}$.
- Risk: If powered via USB-C and the cable voltage sags slightly (e.g., to 4.75V), $V_{IN}$ to the LDO drops to ~4.35V. This falls below the minimum required input voltage ($3.3\text{V} + 1.1\text{V} = 4.4\text{V}$), potentially causing 3.3V brownouts during ESP32-S3 Wi-Fi TX current spikes.
- Fix: Replace U2 with a lower-dropout LDO (e.g., AP2112K-3.3 or TLV1117LV33, both with $V_{DO} \le 0.4\text{V}$).
