# Grill: per-slot MIDI channel for the four note buttons
Date: 2026-06-07

## Intent
Add a menu option to set the MIDI channel the four note/controller buttons
transmit on, and persist that channel per save-slot so each preset carries its
own pad channel. The buttons already send on `settings.midiChannel`
([sendMidi.h:36](../src/sendMidi.h#L36)) — what's missing is a UI to edit it and
persistence of that value into the slot data.

## Key decisions
- **One shared channel for all four buttons** (not per-button). Reason: note says
  "the midichannel" (singular) and `settings.midiChannel` already exists as one
  shared value. Per-button rejected: 4× storage, 4 edit screens, more flash, and
  no concrete need — pads fire fixed notes 0–3 today.
- **Per-slot, not global.** Reason: user's mental model is "pad channel travels
  with the preset." Loading a slot sets the button channel from that slot.
- **Accept invalidation of existing saved presets / no migration code.** Reason:
  personal dev board, re-saving 4 slots after flashing is trivial; migration code
  costs scarce flash. Mitigation in implementation: read-back guard so a virgin
  or old-format slot (EEPROM 0xFF) falls back to channel 1 (stored 0) instead of
  an out-of-range channel.
- **Menu placement: new pos 3**, grouped with the other channel setters
  (knob CH = 1, all-knobs CH = 2, button CH = 3); clear/save/load shift down by
  one. `MENU_ITEMS` 7 → 8.
- **Edit interaction mirrors "Set all Channels" (pos 2):** min/max buttons nudge
  1–16 with wrap, stored 0–15, displayed +1. No active-knob sweep — twitchy and
  unnecessary for a one-shot setting.

## Surfaced assumptions
- `settings.midiChannel` is stored 0–15 (DIN adds +1, USB raw) — confirmed in
  [midiIO.h:13](../src/io/midiIO.h#L13). Edit screen must show +1.
- Editing the channel changes runtime behaviour immediately; it only becomes
  permanent when the user runs "Save Config" — same as knob CC/CH today.

## Implementation notes
- Store the 4 per-slot button channels in a **separate EEPROM region** (after the
  `settings` byte at 900, e.g. 904–907) so the existing knob-slot layout at
  addr 1..497 is untouched — cleaner than widening `knobConfig_t`, and makes the
  "break" harmless (only new bytes added).
- `saveConfig(slot)` writes `settings.midiChannel`; `loadConfig(slot)` reads it
  back into `settings.midiChannel` with the >15 → 0 guard.

## Out of scope
- Per-button channels.
- What notes the buttons send (still fixed 0–3).
- Fixing the existing global-`settings` default (1 → displays as ch 2) or USB-vs-
  DIN channel-offset convention.
- Verification: requires flashing to hardware and re-saving slots; can't be
  tested in-repo.
