void getClock(){

  readClock = readClockPin;
  
  // if readClockPin reads a falling edge (1->0) then stop INT1
  if (readClock == 0 && lastClock == 1) {
    if (internalClockIsRunning) {
      TIMSK1 = 0x0;
      internalClockIsRunning = false;
      lastClockFallingEdge = millis();
    }
    newClock = 1;
  }

  // if last clock falling edge was over 1 second ago enable INT1
  if (!internalClockIsRunning) {
    if (lastClockFallingEdge - millis() > 1000) {
      internalClockIsRunning = true;
      TIMSK1 |= (1 << OCIE1A);
    }
  }
  lastClock = readClock;
}
