void doNoteFunction(uint16_t noteToDac) {

  // write new note if...
  // glide is off...
  if (!glideIndex ||

      // or glideLegato is active, and only one key is pressed...
      (glideIndex && !mode && glideLegato && activeNotePlates < 2) ||

      // or arpeggiator and glideLegato is active but arpGlide is false
      (glideIndex && mode && glideLegato && !arpGlide)) {
    writeDAC(noteToDac);
    glideNote = noteToDac;
    // glideNote = notes[activeNote] + octave;
  }

  // glide function
  if ((glideIndex && !glideLegato) || (glideIndex && !mode && glideLegato && activeNotePlates >= 2) || (glideIndex && mode && glideLegato && arpGlide)) {
    glideDetermination(noteToDac);
  }

}
