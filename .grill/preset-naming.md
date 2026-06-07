# Grill: Preset naming (savestate names)
Date: 2026-06-07

## Intent
Let the user give each of the 4 save slots a human-readable name (capital letters),
shown x-centered on startup and on menu-load beneath "Loading Slot N", revealed
letter-by-letter like the existing "KNOBZ" splash.

## Constraints
- Flash budget is the gating constraint (28672-byte cap). This is a "dream feature" —
  keep code minimal: F() strings, `byte` types, no Arduino String.
- EEPROM is not constrained — plenty of free space; names park in the free region
  after the button-channel bytes (BUTTON_CHANNEL_EEPROM_ADDR 904-907).

## Key decisions
- Decision: **No standalone "Name preset" menu item.** Naming is folded into the
  existing Save Config flow (menu.h case 6): pick slot -> enter name -> hold set ->
  saveConfig writes knob config + name together (atomic, can't drift).
  Reason: cleaner data, name+config always in sync. Alternative rejected: separate
  menu item per the original devnote (would let name and config drift out of sync).
- Decision: **Input routine** — min button = cursor left, max button = cursor right.
  ANY knob moving rewrites the character at the cursor live (knob 0-127 scaled to the
  alphabet). Hold the mode/"set" button >=2 s -> name blinks -> confirm + save.
  Reason: reuses existing knob-edit idiom (menu.h CC editing).
- Decision: **No abort.** Once in the routine you commit to a name; only passive exit
  is MENU_TIMEOUT. Short tap of set does nothing destructive.
- Decision: **No per-letter commit.** The char at the cursor is just the last knob
  twist; you move on with min/max. Only the whole name is confirmed (hold set).
- Decision: **Alphabet = 27 entries: index 0 = blank/space, then A-Z.** Knob at
  minimum = blank, so shorter names work and any position is erasable. Trailing
  blanks trimmed when computing centered width.
- Decision: **Name length = 12 chars** fixed buffer per slot. 4 x 12 = 48 bytes
  EEPROM. 12 x 8 px (ZevvPeep8x16) = 96 px, centers within 128 px.
- Decision: **Entry starts blank** (not preloaded with the slot's current name).
- Decision: **Unnamed / legacy slot (EEPROM 0xFF) displays blank** — just
  "Loading Slot N", nothing on the lower line. Mirrors the existing 0xFF button-channel
  guard in controller.h:168.
- Decision: **Startup/menu-load reveal** — name on the lower line, x-centered, revealed
  letter-by-letter reusing the ~60 ms/char cadence of the KNOBZ splash (main.cpp:129-133).

## Surfaced assumptions
- The "set button" is physically the mode/menu button (`modeButton`, shift bit 6) — the
  same button that means back/exit elsewhere. Hold-to-confirm avoids colliding with its
  tap=back behavior.
- Names stored in their own EEPROM region (suggest NAME_EEPROM_ADDR ~908, 12 bytes/slot),
  independent of the knobConfig_t blocks (addr 1-496) and settings (900).

## Out of scope
- Lowercase, digits, symbols (capitals + blank only).
- Editing/renaming a slot without re-saving its config (naming only happens through Save).
- A standalone "Name preset" menu item.
