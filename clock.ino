void getClock(){

  readClock = readClockPin;

  // if readClockPin reads a falling edge (1->0) then stop INT1
  if (readClock == 0 && lastClock == 1) {
    if (internalClockIsRunning) {
      TIMSK1 = 0x0; // disable INT1
      internalClockIsRunning = false;
    }
    lastClockFallingEdge = millis();
    newClock = 1;
  }

  // if internal clock should run, but it isn't (probably because of external clock present)
  if (!internalClockIsRunning && internalClockToggle == CLOCK_ENABLED) {
    // if last clock falling edge was over 1 second ago enable INT1
    if (lastClockFallingEdge - millis() > 1000) {
      internalClockIsRunning = true;
      TIMSK1 |= (1 << OCIE1A); // enable INT1
    }
  }
  lastClock = readClock;
}
