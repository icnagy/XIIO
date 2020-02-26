void arpFunction(){

  if (newClock && seqLength > 0 || (newClock && seqLength > 0 && freeze)){
    
    switch (seqDirection){

      // up
      case 0:
      seqIndex++;
      if (seqIndex > seqLength){
        seqIndex = 1;
        doRandomOct = 1;
      }
      break;

      // down
      case 1:
      seqIndex--;
      if (seqIndex < 1){
        seqIndex = seqLength;
        doRandomOct = 1;
      }
      break;

      // ping-pong
      case 2:
      if (seqLength == 1){
        seqIndex = 1;
      }
      else{
        bool changeDirection = 0;
        if (pingpongDirection == 1){
          seqIndex++;
          if (seqIndex >= seqLength){
            seqIndex = seqLength;
            changeDirection = 1;
          }
        }
        if (pingpongDirection == 0){
          seqIndex--;
          if (seqIndex <= 1){
            changeDirection = 1;
            doRandomOct = 1;
          }
        }
        if (changeDirection == 1){
          pingpongDirection = !pingpongDirection;
        }
      }
      break;

      // random
      case 3:
      seqIndex = random (1, seqLength+1);
      break;
      
    }

    // refrsh CV output
    if (!arpRandomScope){
      if (octaveAction == queue){
        doNoteFunction(notes[seqNotes[seqIndex-1]]+queuedOctave);
      }
      else {
        doNoteFunction(notes[seqNotes[seqIndex-1]]+octave);
      }
    }
    else{
      arpRandom();
    }
    
    // do trigger
    doTriggerFunction(1);
    
  }
}

