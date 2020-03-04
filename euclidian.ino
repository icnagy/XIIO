// +/- selects channel 1 - 3 => blink channel number?
// switch 1 and 2 adjusts pulses
// note plates rotates the rythm?

// Todo:
// [] adjustable pattern length

uint8_t selectedChannel = 0;
uint8_t euArray[numberOfEuclidianChannels][patternLength];

uint8_t numberOfSteps[numberOfEuclidianChannels] = { patternLength, patternLength, patternLength };
uint8_t numberOfPulses[numberOfEuclidianChannels] = { 4, 2, 8};
uint8_t inRotate[numberOfEuclidianChannels] = { 0, 4, 0 };

uint32_t lastTriggerRisingEdgeTime[numberOfEuclidianChannels];
bool eucTriggerState[numberOfEuclidianChannels];

void eucFunction() {
  if (newClock) {
    newClock = 0;

    // Channel 0 is the old trigger out
    int myPulse = (currPulse + inRotate[0]) % numberOfSteps[0];
    if (euArray[0][myPulse] > 0) {
      eucTriggerState[0] = HIGH;
      lastTriggerRisingEdgeTime[0] = millis();
      doTriggerFunction(1);
      Serial.print("1");
    }
    else
      Serial.print("0");
    // Channel 1 is the switch 1 output
    myPulse = (currPulse + inRotate[1]) % numberOfSteps[1];
    if (euArray[1][myPulse] > 0) {
      eucTriggerState[1] = HIGH;
      lastTriggerRisingEdgeTime[1] = millis();
      switch0high;
      Serial.print("1");
    }
    else
      Serial.print("0");
    // Channel 2 is the switch 2 output
    myPulse = (currPulse + inRotate[2]) % numberOfSteps[2];
    if (euArray[2][myPulse] > 0) {
      eucTriggerState[2] = HIGH;
      lastTriggerRisingEdgeTime[2] = millis();
      switch1high;
      Serial.println("1");
    }
    else
      Serial.println("0");
  }

  if (eucTriggerState[1] && millis() - lastTriggerRisingEdgeTime[1] >= 10) { // trigger width = 5mS
    eucTriggerState[1] = LOW;
    switch0low;
  }
  if (eucTriggerState[2] && millis() - lastTriggerRisingEdgeTime[2] >= 10) { // trigger width = 5mS
    eucTriggerState[2] = LOW;
    switch1low;
  }
}

void euCalc(int channel) {
  int loc = 0;

  // clear the array to start
  for (int i=0; i<patternLength; i++) {
    euArray[channel][i] = 0;
  }

  if ((numberOfPulses[channel] >= numberOfSteps[channel]) || (numberOfSteps[channel] == 1)) {
    if (numberOfPulses[channel] >= numberOfSteps[channel]) {
      for (int i = 0; i < numberOfSteps[channel]; i++) {
        euArray[channel][loc] = 1;
        loc++;
      }
    }
  } else {
    int offs = numberOfSteps[channel] - numberOfPulses[channel];
    if (offs >= numberOfPulses[channel]) {
      int ppc = offs / numberOfPulses[channel];
      int rmd = offs % numberOfPulses[channel];

      for (int i = 0; i < numberOfPulses[channel]; i++) {
        euArray[channel][loc] = 1;
        loc++;
        for (int j = 0; j < ppc; j++) {
          euArray[channel][loc] = 0;
          loc++;
        }
        if (i < rmd) {
          euArray[channel][loc] = 0;
          loc++;
        }
      }
    } else {
      int ppu = (numberOfPulses[channel] - offs) / offs;
      int rmd = (numberOfPulses[channel] - offs) % offs;

      for (int i = 0; i < offs; i++) {
        euArray[channel][loc] = 1;
        loc++;
        euArray[channel][loc] = 0;
        loc++;
        for (int j = 0; j < ppu; j++) {
          euArray[channel][loc] = 1;
          loc++;
        }
        if (i < rmd) {
          euArray[channel][loc] = 1;
          loc++;
        }
      }
    }
  }
  Serial.println();
  for (int i=0; i<patternLength; i++) {
    Serial.print(euArray[channel][i]);
  }
  Serial.println();
}
