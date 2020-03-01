README.md

## What's new in V3

V3 adds the following new features:
- Internal clock 60-240 bpm triggering on quater or sixteenth notes
- Rewritten glide function with selectable glide time from 1/32note to full note
- Expanded settings page via multi-plate touch

###  Internal clock

CNTR1/INT1 has been repurposed as an adjustable internal trigger generator to drive the arp / sequencer.
BPM is adjustable by the encoder while in keyboard/note mode.
The current BPM setting is saved with the preset.
Quantization can be quater not or sixteenth note which is also saved with the preset.

### Rewritten glide

With the new glide implementation it is possible to select the folowing glide times:
- 1/32 note (XOOO)
- 1/16 note (OXOO)
- 1/8 note  (XXOO)
- 1/4 note  (OOXO)
- 1/2 note  (XOXO)
- Full note (OXXO)

### Expanded settings

Example: to access internal clock enable/disable settings:

1. go to settings
2. press and hold octave - and octave + plates at the same time
3. use the encoder to toggle between
   XXOO disabled
   OOXX enabled

To set internal clock's quantization time:

1. go to settings
2. press and hold octave + and freeze plates at the same time
3. use the encoder to toggle between
   XXOO 1/4 note,
   OOXX 1/16 note

## Development

```
arduino-cli compile --fqbn arduino:avr:nano && \
  avrdude -B 1 -V -p m328p -c usbasp -P usb -U flash:w:XIIO_v2_firmware.arduino.avr.nano.hex:i -U flash:w:XIIO_v2_firmware.arduino.avr.nano.with_bootloader.hex:i -U lfuse:w:0xff:m -U hfuse:w:0xda:m -U efuse:w:0xfd:m
```
