void edit(int8_t vari) {

  switch (page) {

    case keyboard_page:

      if (notePlates && mode == keyboard) {
        notes[activeNote] = finibus((notes[activeNote] + vari * 12), 288, 708);
        writeDAC(notes[activeNote] + octave);
        doTriggerFunction(1);
        return;
      }
      else
      {
        // BPM adjust
        internalClockBPMIndex = finibus(internalClockBPMIndex + vari, 0, 180);      // limit to the 60-240 BPM
        // Since we just changed the internal clock speed, adjust the glide time in ticks accordingly
        totalGlideTicks = _32noteTicks[internalClockBPMIndex] * GlideTimeMultiplier[glideTime];

        if(internalClockIsRunning && internalClockToggle == CLOCK_ENABLED) {
          initializeInterrupts();
        }
      }
      break;
    case settings_page:
      settings(vari);
      return;
      break;
  }
}

void settings(int8_t vari) {

  switch (plates) {
    case 3: // octave - and + plates
      internalClockToggle = finibus(internalClockToggle + vari, 0, 1);
      if(internalClockToggle == CLOCK_ENABLED) {
        internalClockIsRunning == true;
        TIMSK1 |= (1 << OCIE1A); // enable INT1
      }
      else {
        TIMSK1 = 0x0; // disable INT1
        internalClockIsRunning == false;
      }
      GL = B1100 >> (internalClockToggle << 1);
      break;
    case 6: // octave + and freeze plates
      if(internalClockToggle == CLOCK_ENABLED) {
        // set quater note or sixteenth note quant for internal clock
        internalClockQuantTime = finibus(internalClockQuantTime + vari, 0, 1);
        G = B1100 >> (internalClockQuantTime << 1);
        initializeInterrupts();
      }
      break;
    case 12: // switch 1 & switch 2
      if(internalClockToggle == CLOCK_ENABLED) {
        // toggle trigger out for internal clock
        internalClockEmmitsTrigger = finibus(internalClockEmmitsTrigger + vari, 0, 1);
        G = B1100 >> (internalClockEmmitsTrigger << 1);
      }
      break;
    case 0:
      WL = B1111;
      break;

    case 1: // octave - plate
      // octave behavior
      // 0 -> latching
      // 1 -> 1 oct. momentary
      // 2 -> 2 oct. momentary
      octaveBehavior = finibus (octaveBehavior + vari, 0, 2);
      W = B1100 >> octaveBehavior;
      break;

    case 2: // octave + plate
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

    case 4: // switch plate 1
      // switch 1 behavior
      // 0 -> momentary
      // 1 -> trigger
      // 2 -> latching
      switchPlateBehavior[0] = finibus ((switchPlateBehavior[0] + vari), 0, 2);
      if (enableFreeze && mode) {
        switchPlateBehavior[0] = latching_switch;
      }
      R = B1100 >> switchPlateBehavior[0];
      break;

    case 8: // switch plate 2
      // switch 2 behavior
      // 0 -> momentary
      // 1 -> trigger
      // 2 -> latching
      switchPlateBehavior[1] = finibus ((switchPlateBehavior[1] + vari), 0, 2);
      if (arpGlide && mode) {
        switchPlateBehavior[1] = momentary_switch;
      }
      R = B1100 >> switchPlateBehavior[1];
      break;

    case 256: // note plate 5
      // glide time
      // 0 Off, 1-6 1/32 - full note
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
        totalGlideTicks = _32noteTicks[internalClockBPMIndex] * GlideTimeMultiplier[glideTime];
        // Serial.println(totalGlideTicks);
      }
      // ledScale(glideTime);
      // return;
      break;

    case 512: // note plate 6
      // glide mode
      // 0 -> always glide
      // 1 -> only when two (or more) notes overlap each other
      glideLegato = finibus ((glideLegato + vari), 0, 1);
      if (glideLegato && mode) {
        switchPlateBehavior[1] = momentary_switch;
      }
      B = B1100 >> glideLegato * 2;
      break;

    case 1024: // note plate 7
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
    case 2048: // note plate 8
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

    case 16: // note plate 1
      if ((vari == -1 && (mode > 0) || vari == 1 && (mode < 2)) && freeze) { // switch off freeze, when switching between modes
        freeze = 0;
        switch0low;
        seqLength = 0;
        moveSeq = 0;
        queuedOctave = octave;
      }
      mode = finibus ((mode + vari), 0, 3);
      if(mode != euclidian) {
        if (glideLegato && mode) {
          switchPlateBehavior[1] = momentary_switch;
        }
        if (enableFreeze && mode) {
          switchPlateBehavior[0] = latching_switch;
        }
      }
      Y = B1000 >> mode;
      break;

    case 32: // note plate 2
      seqDirection = finibus ((seqDirection + vari), 0, 3);
      Y = B1000 >> seqDirection;
      break;

    case 64: // note plate 3
      enableFreeze = finibus ((enableFreeze + vari), 0, 1);
      if (enableFreeze && mode) {
        switchPlateBehavior[0] = latching_switch;
      }
      if (!enableFreeze) {
        freeze = 0;
        seqLength = 0;
      }
      Y = B1100 >> enableFreeze * 2;
      break;

    case 128: // note plate 4
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
