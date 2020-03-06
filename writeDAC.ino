uint16_t buffer[5];
uint8_t data_available = 0;

void writeDAC (uint16_t data){
  uint16_t command = 0x1000 | (data << 2 & 0x0FFF);
  buffer[data_available++] = command;
  if(data_available > 4) {
    data_available = 4;
  }
}

ISR(TIMER2_COMPA_vect){                           //interrupt commands for TIMER 2 here

  if(gliding == GLIDE_ON) {

    // calculate new gliding note
    if (glideDirection == GLIDE_DIRECTION_UP)
    {
      glide_accumlator = glide_accumlator + glide_step_per_tick;
    }
    else {
      glide_accumlator = glide_accumlator - glide_step_per_tick;
    }
    glideNote = glide_accumlator >> 10;

    // should we stop gliding?
    if ((glideNote == glideStop) ||                       // are we there yet?
        (glideDirection == GLIDE_DIRECTION_DOWN && glideNote < glideStop) || // did we overshoot on our way down?
        (glideDirection == GLIDE_DIRECTION_UP && glideNote > glideStop) ||   // did we overshoot on our way up?
        glideNote > 708 ||                                // over highest note?
        glideNote < 288) {                                // under lowest note?
      glideNote = glideStop;
      // if yes, switch of glide
      gliding = GLIDE_OFF;
    }
    writeDAC(glideNote);
  }

  int read_pointer = 0;
  while(data_available > 0) {
    uint16_t command = buffer[read_pointer++];
    SPI.setDataMode(SPI_MODE0);
    PORTB &= ~_BV(PB2);                           // set CS pin (D10) low, equals to "digitalWrite(10,LOW);"
    SPI.transfer(command>>8);
    SPI.transfer(command&0xFF);
    PORTB |= _BV(PB2);                            // set CS pin (D10) high, equals to "digitalWrite(10,HIGH);"
    data_available--;
  }
}
