void arpRandom() {

  // random octave calculated at the end of array
  // 1 > +1 oct
  // 2 > +2 oct
  // 3 > -1/+1 oct
  // random octave calculated at each new note
  // 4 > +1 oct
  // 5 > +2 oct
  // 6 > -1/+1 oct

  switch (arpRandomScope) {
    case 1:
      if (doRandomOct == 1) {
        randomOct = random(1, 3);
        randomOct--;
        doRandomOct = 0;
      }
      break;
    case 2:
      if (doRandomOct == 1) {
        randomOct = random(1, 4);
        randomOct--;
        doRandomOct = 0;
      }
      break;
    case 3:
      if (doRandomOct == 1) {
        randomOct = random(0, 3);
        randomOct--;
        doRandomOct = 0;
      }
      break;
    case 4:
      randomOct = random(1, 3);
      randomOct--;
      break;
    case 5:
      randomOct = random(1, 4);
      randomOct--;
      break;
    case 6:
      randomOct = random(0, 3);
      randomOct--;
      break;
  }

  if (octaveAction == queue) {
    randomOctValue = queuedOctave + (randomOct * 144);
  }
  else {
    randomOctValue = octave + (randomOct * 144);
  }

  randomOctValue = finibus (randomOctValue, -288, 288);

  /*

  if (randomOctValue < -288) {
    randomOctValue = -288;
  }
  if (randomOctValue > 288) {
    randomOctValue = 288;
  }

  */

  doNoteFunction(notes[seqNotes[seqIndex - 1]] + randomOctValue);


}

