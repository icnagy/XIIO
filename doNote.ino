void doNoteFunction(uint16_t noteToDac) {
  Serial.print("noteToDac: "); Serial.println(noteToDac);
  // write new note if...
  if (!glideEnabled ||                                                  // glide is off...
       (glideEnabled && !mode && glideLegato && activeNotePlates < 2) ||// or glideLegato is active, and only one key is pressed...
       (glideEnabled && mode && glideLegato && !arpGlide)) {            // or arpeggiator and glideLegato is active but arpGlide is false
    writeDAC(noteToDac);
    glideNote = noteToDac;
  }

  // glide function
  if ((glideEnabled && !glideLegato) || 
      (glideEnabled && !mode && glideLegato && activeNotePlates >= 2) || 
      (glideEnabled && mode && glideLegato && arpGlide)) {

    // Serial.println("Setup Glide");
    // Serial.print("Glide note          "); Serial.println(glideNote);

    // get the note to which we are gliding to
    glideStop = noteToDac;
    // Serial.print("Glide stop          "); Serial.println(glideStop);
    // if it's the same, return
    if (glideNote == glideStop){
      return;
    }
    glideCounter = 0;
    // Setup glide params for INT2
    // needs: 
    // glide_step_per_tick = (noteToDac - glideNote) / glide_time_ticks

    // direction of sweep
    // up
    if (glideNote < glideStop){
      glideDirection = GLIDE_DIRECTION_UP;

      glide_step_per_tick = ((uint32_t)(noteToDac - glideNote) << 10) / totalGlideTicks;
      // Serial.print("glideDelta          "); Serial.println(noteToDac - glideNote);
      // Serial.print("glideDelta << 10    "); Serial.println((uint32_t)(noteToDac - glideNote)<<10);
      glide_accumlator = (((uint32_t)glideNote) << 10) + glide_step_per_tick;

    }
    else {
      // down
      glideDirection = GLIDE_DIRECTION_DOWN;      
      glide_step_per_tick = ((uint32_t)(glideNote - noteToDac) << 10) / totalGlideTicks;
      // Serial.print("glideDelta          "); Serial.println(glideNote - noteToDac);
      // Serial.print("glideDelta << 10    "); Serial.println((uint32_t)(glideNote - noteToDac)<<10);
      glide_accumlator = (((uint32_t)glideNote) << 10) - glide_step_per_tick;
    }

    // Serial.print("totalGlideTicks     "); Serial.println(totalGlideTicks);
    // Serial.print("Glide step per tick "); Serial.println(glide_step_per_tick);
    // Serial.print("Glide accumlator    "); Serial.println(glide_accumlator);
    gliding = GLIDE_ON;

    // calculate new gliding note
    glideNote = glide_accumlator >> 10;

    // Serial.print("Glide note          "); Serial.println(glideNote);
    // Serial.println("=========");

    // refresh DAC  
    writeDAC(glideNote);
  }
}
