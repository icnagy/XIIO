void doNoteFunction(uint16_t noteToDac) {
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

    // get the note to which we are gliding to
    glideStop = noteToDac;
    // if it's the same, return
    if (glideNote == glideStop){
      return;
    }
    // Setup glide params for INT2, so it can increment/decrememnt glideNote
    // in each tick with glide_step_per_tick
    // glide_step_per_tick = (noteToDac - glideNote) / glide_time_ticks

    // direction of sweep
    if (glideNote < glideStop){  // up
      glideDirection = GLIDE_DIRECTION_UP;

      glide_step_per_tick = ((uint32_t)(noteToDac - glideNote) << 10) / totalGlideTicks;
      glide_accumlator = (((uint32_t)glideNote) << 10) + glide_step_per_tick;
    }
    else {  // down
      glideDirection = GLIDE_DIRECTION_DOWN;

      glide_step_per_tick = ((uint32_t)(glideNote - noteToDac) << 10) / totalGlideTicks;
      glide_accumlator = (((uint32_t)glideNote) << 10) - glide_step_per_tick;
    }

    gliding = GLIDE_ON;

    // calculate new gliding note
    glideNote = glide_accumlator >> 10;

    // refresh DAC
    writeDAC(glideNote);
  }
}
