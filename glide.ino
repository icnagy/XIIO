void glideDetermination(uint16_t noteToDac){

  // get the note to which we are gliding
  glideStop = noteToDac;

  // if it's the same, return
  if (glideNote == glideStop){
    return;
  }

  // get direction of sweep
  // down
  if (glideNote > glideStop){
    glideDirection = 0;
  }
  // up
  if (glideNote < glideStop){
    glideDirection = 1;
  }

  gliding = 1;
  TIFR1 = 0xFF; // reset timer 1
  TIMSK1 = (1 << OCIE1A); // enable interrupts

  // calculate new gliding note
  glideNote = glideNote - 1 + (glideDirection*2);

  // refresh DAC  
  writeDAC(glideNote);


}

void killint(){
  if (gliding){
    TIMSK1 = 0x0;
  }
}

void doint(){
  if (gliding){
    TIMSK1 = (1 << OCIE1A);
  }
}

