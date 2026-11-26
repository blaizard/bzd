---
name: kicad
description: Always use when designing or reviewing schematics, PCBs, BOMs etc with KiCad.
---

Design clean, reliable, and assembly-ready electronic schematics using minimal parts and verified manufacturer reference circuits.

Use `kicad-cli`.

## Schematic & PCB Organization

- Functional clustering: Group all support components (decoupling caps, inductors, feedback dividers, pull-ups) tightly around their parent IC in both the schematic and PCB layout.
- Labeled schematic blocks: Draw a bounding rectangle around each functional sub-circuit in the schematic with a clear title (e.g., `3.3V LDO`, `USB-C Input`, `RS-485 Transceiver`) to make visual review effortless.
- Strict alignment: Snap everything to the grid, align components with neighboring parts, and align cluster rectangles along shared edges with straight 90-degree wires.
- Always show text horizontally, never rotate text to match wire angles or component orientation.
- Do not overlap text, pins, components, wires, etc. Keep everything clustered within their bounds and spaced for readability.

## Reference Designs

- Match official datasheet "Typical Application" schematics, feedback formulas, and passive values.
- Show every pin on schematic symbols—tie unused inputs to valid logic levels and ground exposed thermal pads.

## Minimal BOM

- Choose ICs with built-in USB, ESD protection, and fail-safe receivers to cut external support parts.
- Consolidate passives to standard `0805` values (`10 kΩ`, `4.7 kΩ`, `100 nF`, `10 µF`) across the board.
- Match peripheral logic voltages to the main MCU rail to avoid level shifters and extra regulators.

## SMT Assembly (LCSC / JLCPCB)

- Place all SMD components on the Top layer to qualify for single-sided Economic PCBA.
- Select active, in-stock Basic/Preferred parts—never use NRND (Not Recommended for New Design) or obsolete components.
- Verify Pin 1 and polarity markers against standard pick-and-place orientations.

## Power, Thermals & Pins

- Calculate power dissipation and thermal limits using official datasheet formulas before finalizing regulators.
- Use switching regulators instead of LDOs whenever voltage drops or load currents cause excess heat or battery drain.
- Protect external power ports with bulk reservoir capacitors and self-resetting PTC polyfuses.
- Keep user peripherals off MCU strapping, boot-mode, and internal Flash/PSRAM pins.

# Gotchas

- **Enable / Shutdown Pins:** Pull regulator `EN` and transceiver sleep pins to a safe default state so rails behave predictably during boot and sleep.
- **USB-C CC Pins:** Use two separate `5.1 kΩ` pull-down resistors (`CC1` and `CC2`)—never tie them to one shared resistor.
- **Battery Power-Path:** Isolate Li-Ion charger ICs from system loads using a P-MOSFET and Schottky diode so charging terminates cleanly.
