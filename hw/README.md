# KNOBZ hardware as built

Copied 2026-10-17 from `KNOBZ controller/production files/` (the originals stay there), so DeviceWizard can pin them
by commit. DeviceWizard rebuilds KNOBZ electrically identical on its wood stack (DW spec 17, Phases 10 and 11).

| File | What it is | sha256 |
|---|---|---|
| `Knobz_7e5ea487d96047b5b93cb70bd241256c.zip` | EasyEDA Standard export (2026-10-16): the schematic, the main board, the top plate, the bottom plate, the case dimensions and the hole positions, as JSON | `96aedc1f156a8b2f0b8d726a6f6d884bfd867542c45a82ffdc835409ee08c96e` |
| `Knobz_PCB_Knobz__20251120231733.zip_Y371.zip` | The main board as JLCPCB made it: order Y371, Gerbers and drill (no assembly files) | `fd6f04cb68ef820b891ada92a44e945bff6138df71c47cc28012bbd1623fb013` |
| `Knobz_Knobz_Topplate__20251121214243.zip_Y377.zip` | The top plate as JLCPCB made it: order Y377, Gerbers and drill | `7557bbf291e489a9875d1495f86bf602a6caea9e63539e9ff6038ae1223e0114` |
| `knobz kit bom.xlsx` | The kit's BOM: what the builder gets besides the boards (below) | `755094d507ed2d3b7cb59f29c3571e4c562b5d325019bc8983a6e706a2152b6e` |
| `case.3mf`, `bottom spacer.3mf` | The printed case and bottom spacer as print projects (their STLs are in `printing/`) | `5f9ca7fd…5557`, `da06d2f1…09f4` |

## The kit (from `knobz kit bom.xlsx`)

Besides the front plate, the PCB and the back plate: 20 RV09 pots, 10 faders, the 0.91" 128 × 32 I2C OLED, the
VL53L0X, 3 PJ-3240-5A minijacks, a subminiature toggle (SMTS-102), 14 M2 × 4 pan head screws, 14 M2 × 10 SK screws,
14 M2 × 7 brass standoffs, the printed case and bottom spacer, 20 knobs and 10 fader caps.
