# Alternate firmware for Doboz Audio XIIO

This is an independent firmware update for Doboz Audio XIIO touchplate keyboard Eurorack module. Use at your own risk.

## What's new in V3

- Internal clock 60-240 bpm with selectable quater or sixteenth note quantization
- Rewritten glide function with glide time from 1/32note to full note
- New settings available via multi-plate touch

###  Internal clock

CNTR1/INT1 has been repurposed as an adjustable internal clock generator to drive the arp / sequencer.
BPM is adjustable by the encoder while in keyboard/note mode and no note plate is pressed or in arp/sequencer mode.
Selectable clock resolution: 1/16 note (default, 4 ppqn) and 1/4 note (1ppqn). The current speed and clock resolution is saved with the preset.
Note: adjusting internal clock speed while arp/sequencer is playing will cause dropout.

### Using an external clock

When the internal clock is disabled, the external clock works as before, with the addition of the XIIO calculating the external clock's speed/BPM.

When the internal clock is engaged and an external clock is connected, the internal clock will be disabled, so the external clock takes priority.

When there are no more falling clock signals on the external clock input for over 2 seconds, the internal clock is enabled again.
Note: the speeed of the internal clock will match the external clock's speed.

### Rewritten glide

With the new glide implementation it is possible to select the folowing glide times:
- 1/32 note (XOOO)
- 1/16 note (OXOO)
- 1/8 note  (XXOO)
- 1/4 note  (OOXO)
- 1/2 note  (XOXO)
- Full note (OXXO)

### New settings

To toggle the internal clock:

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

To toggle internal clock's trigger out:

1. go to settings
2. press and hold switch 1 and switch 2 plates at the same time
3. use the encoder to toggle between
   XXOO disable,
   OOXX enable

## Development

```
arduino-cli compile --fqbn arduino:avr:nano && \
  avrdude -B 1 -V -p m328p -c usbasp -P usb -U flash:w:XIIO_v2_firmware.arduino.avr.nano.hex:i -U flash:w:XIIO_v2_firmware.arduino.avr.nano.with_bootloader.hex:i -U lfuse:w:0xff:m -U hfuse:w:0xda:m -U efuse:w:0xfd:m
```
