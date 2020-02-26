void doLeds() {

  switch (page) {

    case keyboard_page:

      // playmode
      // if (!triggerState) {
      if (arpGlide && mode) {

        // show momentary gliding in arp
        switch (switchPlateStatus[1]) {
          case 0:
            B = B | B0000;
            break;
          case 1:
            B = B | B0001;
            break;
        }
      }

      // show switch 1 status when arp glide is disabled
      else {
        switch (switchPlateStatus[1]) {
          case 0:
            R = R | B0000;
            break;
          case 1:
            R = R | B0001;
            break;
        }
      }

      // show freeze status
      if (enableFreeze && mode) {
        switch (switchPlateStatus[0]) {
          case 0:
            B = B | B0000;
            break;
          case 1:
            B = B | B0010;
            break;
        }
      }

      // show switch 0 status when freeze is disabled
      else {
        switch (switchPlateStatus[0]) {
          case 0:
            R = R | B0000;
            break;
          case 1:
            R = R | B0010;
            break;
        }
      }

      // octave status
      if (octave < 0) {
        R = R | B1000;
      }
      if (octave < -144) {
        Y = Y | B1000;
      }
      if (octave > 0) {
        R = R | B0100;
      }
      if (octave > 144) {
        Y = Y | B0100;
      }

      // }

      if (triggerState) {
        switch (mode) {
          case keyboard:
            W = B1000 >> ( activeNote % 4);
            break;
          case arpeggiator:
            W = B1000 >> ( (seqIndex - 1) % 4);
            break;
          case sequencer:
            W = B1000 >> ( (seqIndex) % 4);
            break;
        }

      }
      executeLEDs();
      break;

    case settings_page:
      edit(0);
      break;

  }

}

void ledScale(uint8_t valueToShow) {

  if (!valueToShow) {
    B = B1111;
    executeLEDs();
    return;
  }

  uint8_t ledScaleB[] = {10, 20, 30, 40, 40, 40, 40, 40, 40, 40, 40, 40};
  uint8_t ledScaleG[] = { 0,  0,  0,  0, 10, 20, 30, 40, 40, 40, 40, 40};
  uint8_t ledScaleR[] = { 0,  0,  0,  0,  0,  0,  0,  0, 10, 20, 30, 40};

  uint8_t index;
  index = (valueToShow - 1) % 12;
  // we need 0-11 in 4 circles

  if ( valueToShow < 13 ) {
    leds[3].g = ledScaleR[index];
    leds[3].r = ledScaleG[index];
    leds[3].b = ledScaleB[index];
    leds[2] = 0x000007;
    leds[1] = 0x000007;
    leds[0] = 0x000007;
  }

  if ( valueToShow < 25 && valueToShow > 12 ) {
    leds[3] = LED_WHITE;
    leds[2].g = ledScaleR[index];
    leds[2].r = ledScaleG[index];
    leds[2].b = ledScaleB[index];
    leds[1] = 0x000007;
    leds[0] = 0x000007;
  }

  if ( valueToShow < 37 && valueToShow > 24 ) {
    leds[3] = LED_WHITE;
    leds[2] = LED_WHITE;
    leds[1].g = ledScaleR[index];
    leds[1].r = ledScaleG[index];
    leds[1].b = ledScaleB[index];
    leds[0] = 0x000007;
  }

  if ( valueToShow < 49 && valueToShow > 36 ) {
    leds[3] = LED_WHITE;
    leds[2] = LED_WHITE;
    leds[1] = LED_WHITE;
    leds[0].g = ledScaleR[index];
    leds[0].r = ledScaleG[index];
    leds[0].b = ledScaleB[index];
  }

  for (int i = 0; i < 4; i++) {
    leds[i].nscale8(brightness);
  }
  FastLED.show();

}

void executeLEDs() {

  for (int i = 0; i < 4; i++) {

    leds[i] = LED_BLACK;

    if ((!!(WB & (1 << i))) == 1) {
      leds[i] = LED_WHITE_BLU;
    }
    if ((!!(R  & (1 << i))) == 1) {
      leds[i] = LED_RED;
    }
    if ((!!(RL & (1 << i))) == 1) {
      leds[i] = LED_RED_LOW;
    }
    if ((!!(G  & (1 << i))) == 1) {
      leds[i] = LED_GREEN;
    }
    if ((!!(GL & (1 << i))) == 1) {
      leds[i] = LED_GREEN_LOW;
    }
    if ((!!(B  & (1 << i))) == 1) {
      leds[i] = LED_BLUE;
    }
    if ((!!(BL & (1 << i))) == 1) {
      leds[i] = LED_BLUE_LOW;
    }
    if ((!!(P  & (1 << i))) == 1) {
      leds[i] = LED_PURPLE;
    }
    if ((!!(PL & (1 << i))) == 1) {
      leds[i] = LED_PURPLE_LOW;
    }
    if ((!!(Y  & (1 << i))) == 1) {
      leds[i] = LED_YELLOW;
    }
    if ((!!(YL & (1 << i))) == 1) {
      leds[i] = LED_YELLOW_LOW;
    }
    if ((!!(W  & (1 << i))) == 1) {
      leds[i] = LED_WHITE;
    }
    if ((!!(WL & (1 << i))) == 1) {
      leds[i] = LED_WHITE_LOW;
    }

    leds[i].nscale8(brightness);

  }

  FastLED.show();

  R  = B0000;
  RL = B0000;
  G  = B0000;
  GL = B0000;
  B  = B0000;
  BL = B0000;
  Y  = B0000;
  YL = B0000;
  P  = B0000;
  PL = B0000;
  W  = B0000;
  WL = B0000;
  WB = B0000;

}

int binary(uint8_t value) {

  uint8_t bin = 0;
  bin |= (value >> 3) & B0001;
  bin |= (value >> 1) & B0010;
  bin |= (value << 1) & B0100;
  bin |= (value << 3) & B1000;
  return bin;

}

