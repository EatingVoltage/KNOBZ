# Grill: printable enclosure parts in the repo
Date: 2026-10-15

## Intent
Give KNOBZ owners the files to reprint a broken enclosure part themselves, kept in the
public firmware repo next to the code.

## Constraints
- The repo is public on GitHub (GPL-3.0); anything added is published on push.
- STL only for now — no Fusion source (.f3d/STEP) in the repo.

## Key decisions
- Decision: audience is owners replacing parts, not builders. Reason: the PCB, schematic
  and Gerbers aren't public, so a case alone is useless to a from-scratch builder.
  Alternative considered: open-hardware build kit (needs PCB files, BOM, wiring).
- Decision: folder `printing/` with the three Fusion exports and a short README (part,
  quantity, PETG, 0.2 mm, no supports, what the spacer does). Reason: an STL carries no
  print settings; small press-fit parts are where guessed settings fail.
- Decision: keep Fusion's export filenames. Reason: a re-export overwrites cleanly.
- Decision: copy from Desktop, don't move. Reason: non-destructive.

## Surfaced assumptions
- One buttoncap design fits all 7 buttons.
- The "bottom plate" is a PCB, not a printed or laser-cut part — so no extra file needed.
- The STLs are exported Y-up (thin axis is Y); they load on their side in Z-up slicers.

## Open questions
- Print orientation (which face goes down for case and buttoncap) — not stated; affects
  the "no supports" claim once parts are laid flat.
- Fasteners holding the case/spacer/PCB stack (size, count) — not stated.
- License: STLs currently fall under the repo's GPL-3.0 by default; a hardware license
  (CC-BY-SA, CERN-OHL) was not discussed.

## Out of scope
- Fusion source files (.f3d / STEP).
- PCB / bottom-plate design files.
- Pushing to GitHub.
