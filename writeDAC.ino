void writeDAC (uint16_t data){

  /******************************************
  From MCP4811 datasheet, page 22.
  bit15 = 0       --> write do DAC register
  bit14 = 0       --> don't care
  bit13 = 0       --> x2 gain
  bit12 = 1       --> SHDN active mode
  bit11 - bit2    --> mV (data)
  bit0 - bit1 = 0 --> unknown bits by the DAC
  ******************************************/

  killint();
  
  uint16_t command = 0x1000; // this goes to DAC
  data = data << 2; // milli volts
  command = command | (data & 0x0FFF);

  SPI.setDataMode(SPI_MODE0);
  PORTB &= ~_BV(PB2); // set CS pin (D10) low, equals to "digitalWrite(10,LOW);"
  SPI.transfer(command>>8);
  SPI.transfer(command&0xFF);
  PORTB |= _BV(PB2); // set CS pin (D10) high, equals to "digitalWrite(10,HIGH);"

  doint();

}

