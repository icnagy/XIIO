void getClock(){

  readClock = readClockPin;

  // if readClockPin reads a falling edge (1->0) then stop INT1
  if (readClock == 0 && lastClock == 1) {
    if (internalClockIsRunning) {
      TIMSK1 = 0x0; // disable INT1
      internalClockIsRunning = false;
    }
    uint32_t now = millis();
    externalClockDelta = now - lastClockFallingEdge; // calculate delta/bpm for glide speed
    lastClockFallingEdge = now;
    newClock = 1;

    // match external clock with our rough 1/16 note resolution
    // externalClockBPMIndex = finibus( (15000 / (externalClockDelta + 1) - 60), 0, 180);
    if(externalClockDelta  > (_32noteTicks[externalClockBPMIndex] << 1)) {
      externalClockBPMIndex = finibus(externalClockBPMIndex - 1, 0, 180);
    }
    else {
      externalClockBPMIndex = finibus(externalClockBPMIndex + 1, 0, 180);
    }
    internalClockBPMIndex = externalClockBPMIndex;
    // Since external BPM can vary, adjust the glide time in ticks accordingly
    totalGlideTicks = _32noteTicks[externalClockBPMIndex] * GlideTimeMultiplier[glideTime];
    gateOpenTime = gateLengthsNumerator[selectedGateLengthIndex] / (internalClockBPMIndex + 60);
  }

  // if internal clock should run, but it isn't (probably because of external clock present)
  if (!internalClockIsRunning && internalClockToggle == CLOCK_ENABLED) {

    // if last clock falling edge was over 1 second ago enable INT1
    if (millis() - lastClockFallingEdge  > 2000) {
      // internal clock was already set up, no need to re-initialize
      OCR1A = bpmTimeTableArray[internalClockQuantTime][internalClockBPMIndex];
      internalClockIsRunning = true;
      TIMSK1 |= (1 << OCIE1A); // enable INT1
    }
  }
  lastClock = readClock;
}
