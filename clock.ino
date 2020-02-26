void getClock(){

  newClock = 0;
  readClock = readClockPin;
  
  if (readClock == 0 && lastClock == 1){
    newClock = 1;
  }

  lastClock = readClock;
  
}


