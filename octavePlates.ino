void octavePlates(){
  // read octave plates
  octaveRead = 0;
  if (bitRead(plates,0) == 1){octaveRead = -1;}
  if (bitRead(plates,1) == 1){octaveRead = 1;}

  switch (octaveBehavior){
    
    // latching
    case 0:
    if (octaveReadLast == 0 && octaveRead != 0){
      octave = constrain ((octave + octaveRead * 144), -288, 288);
      doChange = 1;
    }
    break;
    
    // momentary +/- 1 oct
    case 1:
    if (octaveRead != octaveReadLast){
      octave = octaveRead * 144;
      doChange = 1;
    }
    break;
    
    // momentary +/- 2 oct
    case 2:
    if (octaveRead != octaveReadLast){
      octave = octaveRead * 288;
      doChange = 1;
    }
    break;
  }

  // when octaveAction > 0, transposition isimmediate
  if (octaveRead != octaveReadLast && octaveAction && notePlates && !mode){
    doNoteFunction(notes[activeNote]+octave);
    if (octaveAction == immediate_onetrig && octaveRead){
      triggerState = 1;
      doTriggerFunction(1);
    }
  }
  if (octaveAction == immediate_twotrig && octaveRead != octaveReadLast && notePlates){
    triggerState = 1;
    doTriggerFunction(1);
  }

  octaveReadLast = octaveRead;
}

