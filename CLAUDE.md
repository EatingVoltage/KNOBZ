# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

> This is an **eatingVoltage Leonardo firmware** device. The shared house style —
> header-as-module pattern, the `src/io/` skeleton, dual USB+DIN MIDI merge, and the
> AREF/mux/discrete-input gotchas — lives in the **evLeonardoFirmware** skill (auto-fires
> here). This file holds only the device-specific details below.

## What this is

Firmware for **KNOBZ**, a USB+DIN MIDI controller by eatingVoltage. Hardware: Arduino Leonardo (ATmega32u4) with 30 multiplexed pots/faders, a VL53L0X lidar "air knob", 7 buttons + a toggle (via a 74HC165 shift register), a 4-pixel WS2812 ring, and an SSD1306 OLED. Built with PlatformIO + Arduino framework.

## Build / upload / monitor

```sh
pio run                 # compile
pio run -t upload       # flash via the custom avrdude/stk500v2 setup in platformio.ini
pio device monitor      # serial monitor (monitor_speed = 31250, the MIDI baud rate)
```

There are **no automated tests** (`test/` holds only the PlatformIO placeholder README). Verification is manual, on hardware.

Key build facts:
- Single env `[env:leonardo]`. USB MIDI is enabled via `build_flags = -DUSB_MIDI` (this replaces the USB CDC serial port — see "Serial" note below).
- A custom board definition lives in `boards/leonardo_knobz.json` (variant `KNOBZ`, USB product string "eatingVoltage KNOBZ"). Upload uses `protocol stk500v2` / `avr109` bootloader touch.
- Flash budget is tight: `maximum_size` 28672 bytes, RAM 2560 bytes. This drives a lot of the coding style (commented-out code instead of deleted, `byte` everywhere, EEPROM-init code disabled to save space). Watch program size when adding features.
- `lib_deps`: Adafruit NeoPixel, MIDI Library, blokaslabs/USBMIDI, VL53L0X.

## Architecture

### Header-as-module pattern (important)
This is a single-translation-unit Arduino project. **Almost all code lives in `.h` files under `src/` and `src/io/`, and each header both declares and defines its functions and its global objects/variables** (e.g. `knob[]`, `mux_in[]`, `menu`, the `Button` instances, the `Ledling_c` LEDs). They are included exactly once, in order, from `src/main.cpp`. There is no separate `.cpp` per module and no header guards on most files.

Consequences to respect when editing:
- **Include order in `main.cpp` is load-bearing.** A header may use globals defined in an earlier-included header without forward declaration (e.g. `button.h` uses `shiftInReadBit` from `shiftIn.h`; `draw.h`/`sendMidi.h` use `knob[]`/`menu`/`oled`). Don't reorder includes casually, and put new modules after their dependencies.
- Don't `#include` these module headers from more than one place — you'll get duplicate-definition link errors.
- `MIDI` is created in `main.cpp` with `MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI)` before the midi headers are included.

### Main loop (`src/main.cpp`)
`loop()` runs a fixed pipeline each frame: read mux → read shift register/buttons → read lidar → `updateKnobs()` → `updateMenu()` → `updateMidi()` (forward in/out) → send button/CC MIDI → update LEDs → draw OLED. There is no `delay()` in the hot path by design; use `myDelay(ms)` instead — it spins while still calling `updateMidi()` so MIDI forwarding never stalls.

### The 31 "knobs" (`src/controller.h`, `src/config.h`)
`KNOB_AMT = 31`. Indices 0–29 are the multiplexed analog inputs; **index 30 is the lidar air-knob**. Each `knob_c` does a running average (`KNOB_AVG_LEN`), maps out a dead zone (`KNOB_DEADZONE`, so pots reliably hit 0 and 127), applies a change threshold (`POT_TRSH`) to kill noise, and exposes a 7-bit `val` plus a `hasNew` flag consumed downstream. Per-knob config = `{midiChannel, midiCC, min, max}`. `activeKnob` (the most recently moved knob) drives the OLED home screen.

### Analog input (`src/io/analogMux.h`)
Two 74HC4067 muxes share 4 select lines (`MUX_S`) and read on `A0`/`A1`; the loop reads 15 positions × 2 = 30 channels into `mux_in[]`. Note `analogReference(EXTERNAL)` is set in `setup()` (3.3 V regulator) — readings assume that reference.

### Digital input (`src/io/shiftIn.h`, `src/io/button.h`)
One 74HC165 yields 8 bits into `inputValues`. Bit map: 0–3 = controller (note) buttons, 4 = min, 5 = max, 6 = mode/menu, 7 = lidar toggle switch. `button.h` wraps raw bits into `Button` structs exposing `fell/rose/pressed/tapped/held/didHold`. Active-low: input `false` means pressed.

### MIDI (`src/io/midiIO.h`, `midiRouting.h`, `usbmidi.h`, `src/sendMidi.h`)
Dual transport, always sent in parallel: DIN via the MIDI Library on `Serial1` (31250 baud) and USB via blokaslabs USBMIDI. `forwardMidiUSBtoUART()` + `forwardUARTMidi()` bridge the two directions (DIN-in is echoed to both USB and DIN-thru). `USBMIDI.poll()` must run every loop.

**MIDI channel convention (easy to get wrong):** channels are stored 0–15 internally, but **sent and displayed as 1–16** (`+1` applied at send time in `sendMidi.h` and in OLED draw code). Keep that offset consistent in any new channel logic. CC values are scaled per-knob through `map(val, 0,127, min,max)` except while the min/max edit buttons are held (then the raw endpoint is sent).

### LEDs (`src/io/ws2812.h`, `Ledling.h`, `animateNeopixel.h`)
4 NeoPixels: index 0 = central hue (derived from all knob values), 1 = USB activity, 2 = out activity, 3 = in activity. `Ledling_c` objects flash on MIDI traffic via `trigger()`. The DIN-out indicator (pixel 2) only lights when a plug is detected on `PLUG_INDIC_PIN` (pin 7).

### OLED + menu (`src/io/oled.h`, `src/draw.h`, `src/menu.h`)
`drawHome()` shows the active knob's CC/CH/min/max and only repaints on change (`redrawOled` flag, `activeKnob` change) to avoid flicker. `menu.h` is a small state machine (`menu.active`/`menu.editing`/`menu.pos`) for 7 items: set knob CC, set knob CH, set all channels, clear all knobs, clear all min/max, save config, load config. Menu values can be nudged with min/max buttons or swept with the active knob. Menu times out after `MENU_TIMEOUT`. Save/load cases run their own blocking input sub-loops (still calling `updateMidi()`), waiting for a controller button to pick slot 0–3.

### Persistence (`src/config.h`, `src/controller.h`)
EEPROM via `EEPROM_writeAnything`/`readAnything` templates. Layout: global `controllerSettings` at `CONTROLLER_CONFIG_EEPROM_ADDR` (900); **4 save slots** of `knobConfig_t[KNOB_AMT]` packed starting after `sizeof(settings)`; the most-recently-used slot number at `RECENT_SLOT_EEPROM_ADDR` (1022). On boot, `setup()` reads the recent slot and `loadConfig()`s it. The first-boot EEPROM-initialization block in `setup()` is **commented out to save flash** — re-enable it deliberately if you need to provision a virgin board.

## Conventions

- Tunable constants (thresholds, timings, counts, brightness) are centralized as `#define`s in `src/config.h` — prefer adding/adjusting there over magic numbers.
- The codebase keeps dead/experimental code commented out rather than deleting it (e.g. knob animation in `controller.h`, the perf-test `setup/loop` at the bottom of `main.cpp`). Match that style only where it aids hardware debugging; don't add noise.
- `devnotes.txt` is the working todo/done log — check it for known issues and intended-but-unfinished behavior (e.g. a noted off-by-one in channel display, OLED flicker when moving two knobs at once).

## Repo note

`.pio/build/` is gitignored. Note that `.pio/libdeps/` (the downloaded library sources) is still committed, so dependency updates will show up as diffs there.
