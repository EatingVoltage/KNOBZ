# Grill: First-boot EEPROM init of all four save slots
Date: 2026-06-07

## Intent
Make the device self-provision on the very first boot of a virgin board: detect the
uninitialized EEPROM and seed sensible defaults into all four save slots + controller
settings, so a fresh board is usable without a manual provisioning step.

## Constraints
- Flash budget is the binding constraint (28672 bytes). This init was originally
  commented out specifically to save flash — re-enabling must fit. Verify with `pio run`.

## Key decisions
- Detection: keep the existing single-byte sentinel `EEPROM.read(RECENT_SLOT_EEPROM_ADDR) > 3`.
  Reason: virgin AVR EEPROM reads 0xFF reliably, so the one byte is a sufficient virgin
  marker. Alternative considered (scanning all knob-config datapoints for 255) rejected —
  user confirmed "255 everywhere" was just describing the virgin state, not a literal
  requirement; broader scan costs flash for no robustness gain.
- Defaults per knob: midiChannel=0, midiCC=i, min=0, max=127 (matches old commented block).
- Drop the stray `EEPROM.write(1023, 0)` from the old commented code — wrong address
  (recent slot is 1022) and redundant; `saveConfig()` writes the recent-slot byte itself.
  After seeding slots 0..3, ensure recent slot = 0.

## Surfaced assumptions
- `controllerBegin()` later re-reads the recent slot and `loadConfig()`s it, so RAM knob[]
  state after init is reloaded from EEPROM — defaults must be written before that runs.
