void edit(int8_t vari) {

  switch (page) {

    case keyboard_page:

      if (notePlates) {
        notes[activeNote] = finibus((notes[activeNote] + vari * 12), 288, 708);
        writeDAC(notes[activeNote] + octave);
        doTriggerFunction(1);
        return;
      }
      else 
      {
        // BPM adjust
        bpm = finibus(bpm + vari, 0, 180);      // limit to the bpmTimeTable array
        Serial.println(bpm + 60);
        TIMSK1 = 0;
        TCCR1A = 0;                                         // set entire TCCR1A register to 0
        TCCR1B = 0;                                         // same for TCCR1B
        TCNT1  = 0;                                         // initialize counter value to 0
                                                            // set compare match register for bpm/60 Hz increments
        OCR1A = bpmTimeTable[bpm];                          // = 16000000 / (256 * freq) - 1 (must be <65536)
        TCCR1B |= (1 << WGM12);                             // turn on CTC mode
        TCCR1B |= (1 << CS12) | (0 << CS11) | (0 << CS10);  // Set CS12, CS11 and CS10 bits for 256 prescaler
        TIMSK1 |= (1 << OCIE1A);                            // enable timer compare interrupt

        TCCR2B = 0;                 // set entire TCCR2B register to 0
        TCCR2A |= (1 << WGM21);     // turn on CTC mode
                                    // set compare match register for 1000 Hz increments
        OCR2A = 249;                // = 16000000 / (64 * 1000) - 1 (must be <256)
        TCNT2  = 0;                 // initialize counter value to 0
        TCCR2A = 0;                 // set entire TCCR2A register to 0
        TCCR2B |= (1 << CS22) | (0 << CS21) | (0 << CS20); // Set CS22, CS21 and CS20 bits for 64 prescaler
        TIMSK2 |= (1 << OCIE2A);    // enable timer compare interrupt

      }

      break;

    case settings_page:

      settings(vari);
      return;

      break;

  }

}

void settings(int8_t vari) {


  switch (activePlate) {

    case 0:
      WL = B1111;
      break;

    case 1:
      // octave behavior
      // 0 -> latching
      // 1 -> 1 oct. momentary
      // 2 -> 2 oct. momentary
      octaveBehavior = finibus (octaveBehavior + vari, 0, 2);
      W = B1100 >> octaveBehavior;
      break;

    case 2:
      // octave action
      // 0 -> when new note
      // 1 -> immediate
      // 2 -> immediate + trigger when octave plate is touched
      // 3 -> immediate + trigger when octave plate is touched an released
      octaveAction = finibus ((octaveAction + vari), 0, 3);
      if (vari != 0) {
        queuedOctave = octave;
      }
      W = B1000 >> octaveAction;
      break;

    case 3:
      // switch 1 behavior
      // 0 -> momentary
      // 1 -> trigger
      // 2 -> latching
      switchPlateBehavior [0] = finibus ((switchPlateBehavior [0] + vari), 0, 2);
      if (enableFreeze && mode) {
        switchPlateBehavior [0] = 2;
      }
      R = B1100 >> switchPlateBehavior[0];
      break;

    case 4:
      // switch 2 behavior
      // 0 -> momentary
      // 1 -> trigger
      // 2 -> latching
      switchPlateBehavior [1] = finibus ((switchPlateBehavior [1] + vari), 0, 2);
      if (arpGlide && mode) {
        switchPlateBehavior [1] = 0;
      }
      R = B1100 >> switchPlateBehavior[1];
      break;

    case 9:
      // glide time
      // 0 Off 1-7
      // gliding is disabled, when index = 0
      newGlideTime = finibus ((newGlideTime + vari), 0, 6);
      B = binary(newGlideTime);

      if (newGlideTime == 0) {
        glideEnabled = GLIDE_OFF;
        gliding = GLIDE_OFF;
      } 
      else {
        glideTime = newGlideTime - 1;
        glideEnabled = GLIDE_ON;
        totalGlideTicks = _32noteTicks[bpm] * GlideTimeMultiplier[glideTime];
        // Serial.println(totalGlideTicks);
      }
      // ledScale(glideTime);
      // return;
      break;

    case 10:
      // glide mode
      // 0 -> always glide
      // 1 -> only when two (or more) notes overlap each other
      glideLegato = finibus ((glideLegato + vari), 0, 1);
      if (glideLegato && mode) {
        switchPlateBehavior [1] = 0;
      }
      B = B1100 >> glideLegato * 2;
      break;

    case 11:
      preset = finibus ((preset + vari), 0, 14);
      if (preset == currentPreset) {
        if (blinkStatus == 1) {
          R = binary(preset + 1);
        }
        else {
          RL = binary(preset + 1);
        }
      }
      else {
        if (blinkStatus == 1) {
          G = binary(preset + 1);
        }
        else {
          GL = binary(preset + 1);
        }
      }
      break;
    case 12:
      preset = finibus ((preset + vari), 0, 14);
      if (preset == currentPreset) {
        if (blinkStatus == 1) {
          R = binary(preset + 1);
        }
        else {
          RL = binary(preset + 1);
        }
      }
      else {
        if (blinkStatus == 1) {
          G = binary(preset + 1);
        }
        else {
          GL = binary(preset + 1);
        }
      }
      break;

    case 5:
      if ((vari == -1 && (mode > 0) || vari == 1 && (mode < 2)) && freeze) { // switch off freeze, when switching between modes
        freeze = 0;
        switch0low;
        seqLength = 0;
        moveSeq = 0;
        queuedOctave = octave;
      }
      mode = finibus ((mode + vari), 0, 2);
      if (glideLegato && mode) {
        switchPlateBehavior [1] = 0;
      }
      if (enableFreeze && mode) {
        switchPlateBehavior [0] = 2;
      }
      Y = B1100 >> mode;
      break;

    case 6:
      seqDirection = finibus ((seqDirection + vari), 0, 3);
      Y = B1000 >> seqDirection;
      break;

    case 7:
      enableFreeze = finibus ((enableFreeze + vari), 0, 1);
      if (enableFreeze && mode) {
        switchPlateBehavior [0] = 2;
      }
      if (!enableFreeze) {
        freeze = 0;
        seqLength = 0;
      }
      Y = B1100 >> enableFreeze * 2;
      break;

    case 8:
      switch (mode) {

        case keyboard:
          brightness = finibus ((brightness + vari * 5), 30, 150);
          EEPROM.update(1022, brightness);
          W = B1000;
          B = B0100;
          R = B0010;
          Y = B0001;
          break;

        case arpeggiator:
          arpRandomScope = finibus ((arpRandomScope + vari), 0, 6);
          if (arpRandomScope < 4) {
            Y = B1000 >> arpRandomScope;
          }
          else {
            Y = B1100 >> arpRandomScope - 4;
          }
          break;

        case sequencer:
          seqScope = finibus ((seqScope + vari), 1, 15);
          Y = binary(seqScope);
          break;

      }

      break;

  }

  executeLEDs();

}

