void seqFunction() {
  if (newClock && moveSeq || (newClock && freeze && moveSeq)) {

    switch (seqDirection) {

      case dir_up:
        seqIndex++;
        if (seqIndex > seqScope) {
          seqIndex = 0;
        }
        break;

      case dir_down:
        seqIndex--;
        if (seqIndex < 0) {
          seqIndex = seqScope;
        }
        break;

      case dir_pingpong: {
          bool changeDirection = 0;
          if (pingpongDirection) {
            seqIndex++;
            if (seqIndex >= seqScope) {
              seqIndex = seqScope;
              changeDirection = 1;
            }
          }
          if (!pingpongDirection) {
            seqIndex--;
            if (seqIndex <= 0) {
              changeDirection = 1;
            }
          }
          if (changeDirection) {
            pingpongDirection = !pingpongDirection;
          }
        }
        break;

      case dir_random:
        if (allowRandom) {
          seqIndex = random (0, seqScope + 1);
        }
        else {
          allowRandom = 1;
        }
        break;

    }

    // refrsh CV output

    if (octaveAction == queue) {
      doNoteFunction(notes[seqNotes[seqIndex]] + queuedOctave + seqOctaves[seqIndex]);
    }
    else {
      doNoteFunction(notes[seqNotes[seqIndex]] + octave + seqOctaves[seqIndex]);
    }


    doTriggerFunction(1);

  }
}

void seqGenerator() {

  for (int i = 0; i <= seqScope; i ++) {

    seqNotes [i] = (activeNote + i) % 8;
    seqOctaves [i] = 0;

    if (activeNote + i > 7) {
      seqOctaves [i] += 144;
    }

    if (activeNote + i > 15) {
      seqOctaves [i] += 144;
    }
  }

  queuedOctave = octave;

}

void seqClear() {

  for (int i = 0; i <= 15; i ++) {

    seqNotes [i] = 0;
    seqOctaves [i] = 0;

  }

}

void seqReset() {

  pingpongDirection = 1;
  allowRandom = 0;

  switch (seqDirection) {
    case dir_up:
      seqIndex = -1;
      break;
    case dir_down:
      seqIndex = 1;
      break;
    case dir_pingpong:
      seqIndex = -1;
      break;
    case dir_random:
      seqIndex = 0;
      break;
  }
}

