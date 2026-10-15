# Grill: "Clear all MIN/MAX" resets more than min/max
Date: 2026-10-15

## Intent
"Clear all MIN/MAX" should do exactly what it says: min = 0, max = 127 on every knob,
nothing else. Also: no knob should default to CC 0 (Bank Select MSB).

## Key decisions
- Decision: menu case 5 only sets `min = 0` / `max = 127`; drop the copied
  `midiChannel = 0` / `midiCC = i+1` lines. Reason: it was a copy of "Clear all Knobs"
  and silently wiped every knob's CC and channel.
- Decision: RAM only, no auto-save. Reason: matches every other menu item; saving is
  always explicit via "Save Config", and "Load Config" undoes an accidental clear.
  Alternative considered: write the cleared min/max straight into the recent slot.
- Decision: default CC becomes `i+1` (knob 1 -> CC 1 ... air knob -> CC 31) in both
  "Clear all Knobs" (menu case 4) and the first-boot init in `setup()`. Reason: CC 0 is
  Bank Select MSB — turning knob 1 changed banks on receiving synths. Both places
  changed together so defaults stay identical.

## Surfaced assumptions
- Existing save slots keep whatever CCs they were saved with; only the defaults change.
  A board initialised before this fix still has knob 1 = CC 0 in its slots until the
  user runs "Clear all Knobs" (or re-inits EEPROM) and saves.
- Clear covers all 31 knobs, including the lidar air knob (index 30).
