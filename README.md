# Alternate firmware for Doboz Audio XIIO

This is an independent firmware update for Doboz Audio XIIO touchplate keyboard Eurorack module. Use at your own risk.

## What's new in V3

- Internal clock 60-240 bpm with selectable quater or sixteenth note quantization
- Rewritten glide function with glide time from 1/32note to full note
- Expanded settings page via multi-plate touch

###  Internal clock

CNTR1/INT1 has been repurposed as an adjustable internal trigger generator to drive the arp / sequencer.
BPM is adjustable by the encoder while in keyboard/note mode. Two different resolution: 1/4 note and 1/16 note (1ppqn or 4 ppqn)
The current BPM and its quant setting is saved with the preset.

### Rewritten glide

With the new glide implementation it is possible to select the folowing glide times:
- 1/32 note (XOOO)
- 1/16 note (OXOO)
- 1/8 note  (XXOO)
- 1/4 note  (OOXO)
- 1/2 note  (XOXO)
- Full note (OXXO)

### Expanded settings

Example: to toggle the internal clock:

1. go to settings
2. press and hold octave - and octave + plates at the same time
3. use the encoder to toggle between
   XXOO disabled
   OOXX enabled

To set internal clock's quantization time:

1. go to settings
2. press and hold octave + and freeze plates at the same time
3. use the encoder to toggle between
   XXOO 1/16 note,
   OOXX 1/4 note

## Development

```
arduino-cli compile --fqbn arduino:avr:nano && \
  avrdude -B 1 -V -p m328p -c usbasp -P usb -U flash:w:XIIO_v2_firmware.arduino.avr.nano.hex:i -U flash:w:XIIO_v2_firmware.arduino.avr.nano.with_bootloader.hex:i -U lfuse:w:0xff:m -U hfuse:w:0xda:m -U efuse:w:0xfd:m
```
