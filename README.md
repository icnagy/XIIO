README.md

```
arduino-cli compile --fqbn arduino:avr:nano && avrdude -B 1 -V -p m328p -c usbasp -P usb -U flash:w:XIIO_v2_firmware.arduino.avr.nano.hex:i -U flash:w:XIIO_v2_firmware.arduino.avr.nano.with_bootloader.hex:i -U lfuse:w:0xff:m -U hfuse:w:0xda:m -U efuse:w:0xfd:m
```
