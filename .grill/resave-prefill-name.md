# Grill: pre-fill preset name on resave
Date: 2026-06-07

## Intent
When saving into a slot that already has a name, the name-entry page should
pre-fill with that slot's existing name so you can tweak/keep it instead of
retyping. Also: while holding to confirm, the row-3 center label changes from
"ok" to "hold..." so the user knows to keep holding.

## Key decisions
- Decision: pre-fill is keyed to the TARGET slot `s` (the one being saved into),
  read fresh from EEPROM — not the currently loaded/displayed patch name.
  Reason: literal meaning of "last saved name of that slot"; correct when target
  slot != currently loaded slot. Alternative (pre-fill current on-screen name)
  rejected.
- Decision: cursor starts at position 0 on the pre-filled field. Reason: zero
  extra code (cursor already inits to 0), trivial scroll on a 12-char field.
  Alternative (land on first blank to append) rejected as marginal.
- Decision: empty/unnamed slot (0xFF) keeps today's behavior — blank field,
  cursor 0.
- Decision: keep the free resave shortcut — with the name pre-filled, hold-2s
  confirms the same name without any edit. Not forcing an edit.
- Decision: extract the 0xFF-guarded name read from loadConfig into
  readSlotName(slot); call it from both loadConfig and the save pre-fill.
  Reason: de-dupes code (net-neutral/saves flash) and gives a name-only reader
  that does NOT clobber knob config (loadConfig would).
- Decision: row-3 legend swaps to "hold..." while modeButton is pressed, back to
  "ok" on release. Both legend strings padded to equal width (20 chars) so they
  overwrite cleanly with no row clear / no flicker.

## Surfaced assumptions
- enterName() is only called from the save flow (case 6), so removing its
  internal blank-init and relying on the caller to pre-fill presetName is safe.
- Critical: loadConfig() must NOT be used to peek a slot's name during save — it
  overwrites all knob config. Hence the name-only readSlotName helper.
