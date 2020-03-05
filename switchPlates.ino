void switchPlates() {

  if (page == settings_page) {
    return;
  }

  // if(mode == euclidian) {
  //   switchPlateRead[0] = bitRead(plates, 2);
  //   if (switchPlateRead[0] != 0 && switchPlateLast[0] == 0) {
  //     switchPlateStatus[0] = switchPlateRead[0];
  //     numberOfPulses[selectedChannel] = finibus(numberOfPulses[selectedChannel] - 1, 0, patternLength);
  //     Serial.println(numberOfPulses[selectedChannel]);
  //     euCalc(selectedChannel);
  //   }
  //   switchPlateLast[0] = switchPlateRead[0];

  //   switchPlateRead[1] = bitRead(plates, 3);
  //   if (switchPlateRead[1] != 0 && switchPlateLast[1] == 0) {
  //     switchPlateStatus[1] = switchPlateRead[1];
  //     numberOfPulses[selectedChannel] = finibus(numberOfPulses[selectedChannel] + 1, 0, patternLength);
  //     Serial.println(numberOfPulses[selectedChannel]);
  //     euCalc(selectedChannel);
  //   }
  //   switchPlateLast[1] = switchPlateRead[1];

  //   return;
  // }

  for (int i = 0; i < 2; i++) {
    switchPlateRead [i] = bitRead(plates, 2 + i);

    switch (switchPlateBehavior[i]) {

      // momentary
      case momentary_switch:
        if (switchPlateRead[i] != switchPlateLast[i]) {
          switchPlateStatus[i] = switchPlateRead[i];

          doChange = 1;

          if (i == 1 && mode && glideLegato) {
            arpGlide = switchPlateStatus[i];
            if (!arpGlide) { // switch off gliding
              gliding = GLIDE_OFF;
              writeDAC(glideStop);
            }
          }
        }
        // momentary glide in auto mode

        break;

      // trigger
      case trigger_switch:
        // trigger ON
        if (switchPlateRead[i] == 1 && switchPlateLast[i] == 0) {
          switchPlateStatus[i] = 1;
          switchPlateTriggerTime[i] = millis();
        }
        // trigger OFF
        if (switchPlateStatus[i] == 1 && millis() - switchPlateTriggerTime[i] >= 20) {
          switchPlateStatus[i] = 0;
          doChange = 1;
        }
        break;

      // latching
      case latching_switch:
        if (switchPlateRead[i] == 1 && switchPlateLast[i] == 0) {
          switchPlateStatus[i] = !switchPlateStatus[i];
          doChange = 1;
        }

        // freeze in auto mode
        if (i == 0 && mode && enableFreeze) {
          if (switchPlateRead[0] == 1 && switchPlateLast[0] == 0) {
            freeze = !freeze;
          }
          switchPlateStatus[0] = freeze;
        }
        break;
    }

    // refresh LEDs if change happened
    if (switchPlateRead[i] != switchPlateLast[i]) {
      doChange = 1;
    }

    switchPlateLast[i] = switchPlateRead[i];

    // do outputs if change happened
    if (doChange == 1) {
      if (switchPlateStatus[0] == 0) {
        switch0low;
      }
      if (switchPlateStatus[0] == 1) {
        switch0high;
      }
      if (switchPlateStatus[1] == 0) {
        switch1low;
      }
      if (switchPlateStatus[1] == 1) {
        switch1high;
      }
    }
  }
}

