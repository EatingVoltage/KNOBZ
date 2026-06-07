# Grill: OLED flicker optimization (main screen + menu)
Date: 2026-06-07

## Intent
Kill the two visible OLED flicker sources in normal use:
1. Main/home screen flickers when two knobs move at once because `activeKnob`
   ping-pongs between them, each switch forcing a full `oled.clear()` + repaint.
2. Menu editing screens flicker because every value nudge calls `drawMenu()`,
   which does a full `oled.clear()` + full repaint over I2C (no RAM framebuffer
   in SSD1306Ascii — by design, to save RAM).

## Constraints
- Flash budget is the binding constraint: baseline 27460/28672 bytes (95.8%),
  only ~1212 bytes free. Both changes must fit. RAM is comfortable (1474/2560).
- SSD1306Ascii has no framebuffer, so flicker is inherent to clear+repaint;
  fix = avoid full clears, overwrite only changed regions. Library provides
  `clearField(col,row,n)` and `clear(c0,c1,r0,r1)` for surgical redraws.

## Key decisions
- Decision: Do BOTH the debounce and the menu partial-redraw this round.
  Reason: user wants both. Risk: menu rewrite eats flash — measure after.
  Alternative considered: debounce-only first, then decide — rejected.
- Decision: Main-screen fix = time lockout on `activeKnob` reassignment,
  window 250ms. Reason: cheap (one `long` timestamp), kills the two-knob fight.
  Alternative considered: movement-magnitude priority (B) — rejected, costs
  flash/RAM to solve a problem (deliberate simultaneous two-knob moves) the
  user doesn't actually have.
- Decision: Lockout semantics = timer is "time since the *active* knob last
  moved"; it refreshes on every move of the active knob, so another knob can
  never steal the screen while you're actively working the current one. A
  different knob steals only after 250ms of the active knob being idle. If the
  screen has been idle, grabbing another knob takes it instantly.
  Reason: rock-solid "screen follows the knob I'm playing" with same flash cost.
- Decision: Menu partial-redraw scope = editing cases 0–3 (Knob CC, Knob CH,
  set-all-CH, Button CH — the repeated-nudge flicker culprits) PLUS
  `drawNameEntry` (off devnote list, but the heaviest repeated-flicker source:
  full clear every 150ms blink frame). Reason: same technique, biggest wins.
  Alternative considered: also fix selecting-menu nav repaint — rejected, those
  are one-shot flashes per discrete button press, not worth the code.
  Hook point for debounce: controller.h:88-91 (the `activeKnob = i` branch),
  plus a timestamp refresh when the active knob itself reports hasNew.

## Surfaced assumptions
- Menu flicker is lower priority than main-screen flicker because the menu is
  modal and brief; main-screen flicker happens during normal playing. (User
  agreed with this ranking.)
- The two-knob case is incidental brushing, never a deliberate "sweep two
  specific knobs together" gesture — which is why a simple time lockout is fine.

## Out of scope
- Selecting-menu navigation repaint (one-shot per button press) — left as-is.
- Movement-magnitude / per-knob-delta priority scheme.
