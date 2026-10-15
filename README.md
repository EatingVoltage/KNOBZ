# KNOBZ

**A field-programmable MIDI controller by [eatingVoltage](https://eatingvoltage.com/knobz/): 30 controls, a laser "air knob", and every single one set to its own MIDI channel and CC — right on the device, no editor software needed.**

![KNOBZ front panel](https://eatingvoltage.com/wp-content/uploads/2025/11/knobz-front-1024x334.png)

More info and pictures: **[eatingvoltage.com/knobz](https://eatingvoltage.com/knobz/)** · 🇩🇪🇨🇭 [Deutsch weiter unten](#deutsch)

## Why KNOBZ is fun

- **10 faders + 20 knobs** — enough hands-on control for a whole synth, mixer or DAW session.
- **Laser air knob** — wave your hand over the distance sensor to send a CC. A toggle switch turns it on and off (and sends a note, so you can map that too).
- **Per-control setup** — every fader and knob gets its own MIDI channel and CC number.
- **Range scaling** — hold MIN or MAX and move a control to set its output range, e.g. a filter fader that only sweeps the sweet spot.
- **4 named presets** — save and load the full configuration in one of 4 slots, each with its own name. KNOBZ boots into the slot you used last.
- **4 note buttons** for triggering, muting or launching clips.
- **USB-MIDI and TRS MIDI** — plug-and-play on any computer (class compliant, no driver), and a MIDI interface on top: KNOBZ routes MIDI between USB and its TRS ports. It reads both TRS standards (A and B) and sends TRS A.
- **OLED display + RGB LEDs** — the display shows what you're touching, and the LEDs flash on MIDI traffic.

## Quick guide

| You want to... | Do this |
| --- | --- |
| See a control's settings | Move it. The display shows its number, CC, channel and range. |
| Limit a control's range | Hold **MIN** (or **MAX**) and move the control to the new limit. |
| Open the menu | Press **MODE**. **MIN** = ←, **MAX** = →, **MODE** = ok. |
| Change a value in the menu | Tap or hold **MIN**/**MAX**, or sweep the control you're editing. |
| Leave the menu | Press any note button, or wait 5 seconds. |
| Send all current values (e.g. to sync a synth) | Hold **MODE**. |

Menu items: set knob CC · set knob channel · set all channels · set button channel · clear all knobs · clear all min/max · save config · load config.

**Saving a preset:** choose *Save Config*, pick a slot with one of the 4 note buttons, then name it: any knob picks the letter, **MIN**/**MAX** move the cursor, and holding **MODE** for 2 s saves.

## Updating the firmware (PlatformIO)

You only need a USB cable — no programmer.

1. Install [VS Code](https://code.visualstudio.com/) with the [PlatformIO IDE](https://platformio.org/install/ide?install=vscode) extension (or just [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation/index.html) for the command line).
2. Get the code:
   ```sh
   git clone https://github.com/EatingVoltage/KNOBZ.git
   cd KNOBZ
   ```
   (or *Code → Download ZIP* on GitHub, and unzip it)
3. Plug in KNOBZ and flash:
   ```sh
   pio run -e usb -t upload
   ```
   In VS Code: open the `KNOBZ` folder, then in the PlatformIO sidebar choose **usb → Upload**.

PlatformIO resets KNOBZ into its bootloader, finds the port and writes the firmware. Your saved presets stay, since they live in EEPROM.

**If the upload fails:** close any app that holds the KNOBZ port open (serial monitors, configuration tools), or name the port yourself, e.g. `pio run -e usb -t upload --upload-port COM5` (Windows) or `--upload-port /dev/ttyACM0` (Linux).

The default `leonardo` environment (`pio run -t upload`) is for development and flashes through an ISP programmer (stk500v2).

---

## Deutsch

**Ein frei programmierbarer MIDI-Controller von [eatingVoltage](https://eatingvoltage.com/knobz/): 30 Regler, ein Laser-„Luftregler“, und jeder einzelne mit eigenem MIDI-Kanal und eigener CC-Nummer, direkt am Gerät einstellbar, ohne Editor-Software.**

### Was KNOBZ kann

- **10 Fader + 20 Drehregler**: genug Regler für einen ganzen Synth, ein Mischpult oder eine DAW-Session.
- **Laser-Luftregler**: Mit der Hand über dem Abstandssensor einen CC steuern. Ein Kippschalter schaltet ihn ein und aus (und sendet dabei eine Note, die man ebenfalls zuweisen kann).
- **Jeder Regler einzeln einstellbar**: eigener MIDI-Kanal und eigene CC-Nummer pro Fader und Drehregler.
- **Wertebereich begrenzen**: MIN oder MAX halten und Regler bewegen, z. B. damit ein Filter-Fader nur den spannenden Bereich abdeckt.
- **4 Presets mit Namen**: die ganze Konfiguration in einem von 4 Speicherplätzen sichern und laden. KNOBZ startet mit dem zuletzt benutzten Preset.
- **4 Noten-Tasten**: zum Triggern, Muten oder Clips starten.
- **USB-MIDI und TRS-MIDI**: an jedem Computer sofort einsatzbereit (class compliant, kein Treiber), und gleichzeitig ein MIDI-Interface: KNOBZ leitet MIDI zwischen USB und den TRS-Buchsen weiter. Er liest beide TRS-Standards (A und B) und sendet TRS A.
- **OLED-Display + RGB-LEDs**: das Display zeigt, was du gerade bewegst, und die LEDs blinken bei MIDI-Daten.

### Kurzanleitung

| Du willst... | So geht's |
| --- | --- |
| Einstellungen eines Reglers sehen | Regler bewegen. Das Display zeigt Nummer, CC, Kanal und Bereich. |
| Bereich eines Reglers begrenzen | **MIN** (oder **MAX**) halten und Regler auf die neue Grenze stellen. |
| Menü öffnen | **MODE** drücken. **MIN** = ←, **MAX** = →, **MODE** = ok. |
| Wert im Menü ändern | **MIN**/**MAX** tippen oder halten, oder den bearbeiteten Regler bewegen. |
| Menü verlassen | Eine Noten-Taste drücken oder 5 Sekunden warten. |
| Alle aktuellen Werte senden (z. B. Synth synchronisieren) | **MODE** halten. |

Menüpunkte: Knob-CC · Knob-Kanal · alle Kanäle · Tasten-Kanal · alle Knobs zurücksetzen · alle Min/Max zurücksetzen · Preset speichern · Preset laden.

**Preset speichern:** *Save Config* wählen, mit einer der 4 Noten-Tasten den Speicherplatz wählen, dann benennen: ein beliebiger Regler wählt den Buchstaben, **MIN**/**MAX** bewegen den Cursor, und **MODE** 2 Sekunden halten speichert.

### Firmware aktualisieren (PlatformIO)

Du brauchst nur ein USB-Kabel, keinen Programmer.

1. [VS Code](https://code.visualstudio.com/) mit der Erweiterung [PlatformIO IDE](https://platformio.org/install/ide?install=vscode) installieren (oder nur [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation/index.html) für die Kommandozeile).
2. Code holen:
   ```sh
   git clone https://github.com/EatingVoltage/KNOBZ.git
   cd KNOBZ
   ```
   (oder auf GitHub *Code → Download ZIP* und entpacken)
3. KNOBZ anschliessen und flashen:
   ```sh
   pio run -e usb -t upload
   ```
   In VS Code: den Ordner `KNOBZ` öffnen, dann in der PlatformIO-Seitenleiste **usb → Upload** wählen.

PlatformIO startet KNOBZ in den Bootloader neu, findet den Port und schreibt die Firmware. Deine gespeicherten Presets bleiben erhalten, sie liegen im EEPROM.

**Falls der Upload nicht klappt:** Programme schliessen, die den KNOBZ-Port offen halten (serieller Monitor, Konfigurations-Tools), oder den Port selbst angeben, z. B. `pio run -e usb -t upload --upload-port COM5` (Windows) oder `--upload-port /dev/ttyACM0` (Linux).

Die Standard-Umgebung `leonardo` (`pio run -t upload`) ist für die Entwicklung gedacht und flasht über einen ISP-Programmer (stk500v2).

---

## License / Lizenz

GPL-3.0, see / siehe [LICENSE](LICENSE).
