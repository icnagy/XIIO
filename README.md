README.md

## What's new in V3

V3 adds the following new features:
- Internal clock 60-240 bpm triggering on quater notes
- Rewritten glide function with selectable glide time from 1/32note to full note

###  Internal clock

CNTR1/INT1 has been repurposed as an adjustable internal trigger generator to drive the arp / sequencer.
BPM is adjustable by the encoder while in keyboard/note mode.
The current BPM setting is saved with the preset.

### Rewritten glide 

With the new glide implementation it is possible to select the folowing glide times:
- 1/32 note
- 1/16 note
- 1/8 note
- 1/4 note
- 1/2 note
- Full note

## Development

```
arduino-cli compile --fqbn arduino:avr:nano && \
  avrdude -B 1 -V -p m328p -c usbasp -P usb \ 
  -U flash:w:XIIO_v2_firmware.arduino.avr.nano.hex:i \
  -U flash:w:XIIO_v2_firmware.arduino.avr.nano.with_bootloader.hex:i \
  -U lfuse:w:0xff:m -U hfuse:w:0xda:m -U efuse:w:0xfd:m
```
