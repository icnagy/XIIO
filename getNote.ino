int getNote() {

  switch (mode) {
    case euclidian:
      if((notePlates != notePlatesLast) && (inRotate[selectedChannel] != notePlates)){
        inRotate[selectedChannel] = finibus(notePlates, 0, patternLength);
        Serial.println(inRotate[selectedChannel]);
      }
      break;

    /*oooo    oooo oooooooooooo oooooo   oooo oooooooooo.    .oooooo.         .o.       ooooooooo.   oooooooooo.
      `888   .8P'  `888'     `8  `888.   .8'  `888'   `Y8b  d8P'  `Y8b       .888.      `888   `Y88. `888'   `Y8b
       888  d8'     888           `888. .8'    888     888 888      888     .8"888.      888   .d88'  888      888
       88888[       888oooo8       `888.8'     888oooo888' 888      888    .8' `888.     888ooo88P'   888      888
       888`88b.     888    "        `888'      888    `88b 888      888   .88ooo8888.    888`88b.     888      888
       888  `88b.   888       o      888       888    .88P `88b    d88'  .8'     `888.   888  `88b.   888     d88'
      o888o  o888o o888ooooood8     o888o     o888bood8P'   `Y8bood8P'  o88o     o8888o o888o  o888o o888bood8P'  */

    case keyboard:

      activeNotePlates = 0;

      for (int i = 0; i <= 7; i++) {

        // compare previous data with the latest datat from MPR121
        if (bitRead(notePlates, i) == 1 && bitRead(notePlatesLast, i) == 0) {
          activeNote = i;
          doNote = 1;
        }

        // get the number of active note plates
        if (bitRead(notePlates, i) == 1) {
          activeNotePlates++;
        }
      }

      // do CV and trigger
      if (doNote) {
        doNoteFunction(notes[activeNote] + octave);
        doTriggerFunction(1);
      }
      break;

    /*      .o.       ooooooooo.   ooooooooo.   oooooooooooo   .oooooo.      .oooooo.    ooooo       .o.       ooooooooooooo   .oooooo.   ooooooooo.
           .888.      `888   `Y88. `888   `Y88. `888'     `8  d8P'  `Y8b    d8P'  `Y8b   `888'      .888.      8'   888   `8  d8P'  `Y8b  `888   `Y88.
          .8"888.      888   .d88'  888   .d88'  888         888           888            888      .8"888.          888      888      888  888   .d88'
         .8' `888.     888ooo88P'   888ooo88P'   888oooo8    888           888            888     .8' `888.         888      888      888  888ooo88P'
        .88ooo8888.    888`88b.     888          888    "    888     ooooo 888     ooooo  888    .88ooo8888.        888      888      888  888`88b.
       .8'     `888.   888  `88b.   888          888       o `88.    .88'  `88.    .88'   888   .8'     `888.       888      `88b    d88'  888  `88b.
      o88o     o8888o o888o  o888o o888o        o888ooooood8  `Y8bood8P'    `Y8bood8P'   o888o o88o     o8888o     o888o      `Y8bood8P'  o888o  o888o */

    case arpeggiator:
      // get the array of notes for the arpeggiator
      if (!freeze || (freeze && enableFreeze && notePlatesLast < notePlates)) {

        seqLength = 0;

        for (int i = 0; i <= 7; i++) {

          // do an array from active note plates, this will be sequenced
          if (bitRead(notePlates, i) == 1) {
            seqNotes [seqLength] = i;
            seqLength++;
          }
        }

        // reset arp sequence, when no plate is touched
        if (!notePlates) {
          seqIndex = 0;
          seqLength = 0;
          pingpongDirection = 1;
          randomOct = 0;
        }

        // octave refresh at new notes when queued
        if (notePlatesLast != notePlates){
          queuedOctave = octave;
        }
      }
      break;

    /*  .oooooo..o oooooooooooo   .oooooo.      ooooo     ooo oooooooooooo ooooo      ooo   .oooooo.   oooooooooooo ooooooooo.
      d8P'    `Y8 `888'     `8  d8P'  `Y8b     `888'     `8' `888'     `8 `888b.     `8'  d8P'  `Y8b  `888'     `8 `888   `Y88.
      Y88bo.       888         888      888     888       8   888          8 `88b.    8  888           888          888   .d88'
       `"Y8888o.   888oooo8    888      888     888       8   888oooo8     8   `88b.  8  888           888oooo8     888ooo88P'
           `"Y88b  888    "    888      888     888       8   888    "     8     `88b.8  888           888    "     888`88b.
      oo     .d8P  888       o `88b    d88b     `88.    .8'   888       o  8       `888  `88b    ooo   888       o  888  `88b.
      8""88888P'  o888ooooood8  `Y8bood8P'Ybd'    `YbodP'    o888ooooood8 o8o        `8   `Y8bood8P'  o888ooooood8 o888o  o888o */

    case sequencer:

      if (!freeze || (freeze && notePlatesLast < notePlates)) {

        for (int i = 0; i <= 7; i++) {

          // compare previous data with the latest datat from MPR121
          if (bitRead(notePlates, i) == 1 && bitRead(notePlatesLast, i) == 0) {
            activeNote = i;
            doNote = 1;
          }
        }

        if (doNote){
          seqGenerator();
          seqReset();
          moveSeq = 1;
        }

        if (!notePlates){
          moveSeq = 0;
        }


      }
      break;
  }

  // turn off gate when no plate is pressed
  if (!notePlates && gateState) {
    doGateFunction(0);
  }

  if (notePlates && !gateState) {
    doGateFunction(1);
  }

  notePlatesLast = notePlates;

}
