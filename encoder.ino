void encoder() {

  int8_t vari = 0;

  // read encoder
  readEnc = encARead + encBRead;
  if (!encoderDirection) {
    // CW movement
    if (readEnc == 4 && lastEnc == 12) {
      vari--;
    }
    // CCW movement
    if (readEnc == 8 && lastEnc == 12) {
      vari++;
    }
  }
  else {
    // CW movement
    if (readEnc == 4 && lastEnc == 12) {
      vari++;
    }
    // CCW movement
    if (readEnc == 8 && lastEnc == 12) {
      vari--;
    }
  }
  lastEnc = readEnc;

  // do editing of parameters if encoder moved
  if (vari != 0) {
    edit(vari);
  }

  // read encoder switch
  switchState = switchRead;
  if (switchState == 1 && switchHappened == 1) {
    switchHappened = 0; // switch release
  }
  if (switchState == 0 && switchLast == 1) {
    switchTime = millis(); // switch pressed
  }
  if (switchHappened == 0 && switchLast == 0) {
    switchAction = checkSwitch(); // determine switch action
  }
  if (switchAction != switchActionLast && switchAction != 0) {
    doSwitchAction(); // change parameters depending switch action
  }

  switchLast = switchState;
  switchActionLast = switchAction;

}


int checkSwitch() {

  if (switchState == 1) {
    switchHappened = 1;
  }

  // short push and release
  if (switchState == 1 && millis() - switchTime < 400) {
    return 1;
  }

  // more than 1 sec push
  if (switchState == 0 && millis() - switchTime > 1000) {
    return 2;
  }


  return 0;

}

void doSwitchAction() {
  switch (page) {

    case keyboard_page:

      // enter settings page
      if (switchAction == shortpush) {
        page = settings_page;
        activePlate = 0;
      }

      break;

    // while settings page
    case settings_page:

      // exit editing page
      if (switchAction == shortpush) {
        page = keyboard_page;
      }

      // load preset
      if (switchAction == longpush && activePlate == 11) {
        loadPreset();
        loadAnim();
        // page = keyboard_page;
      }

      // save preset
      if (switchAction == longpush && activePlate == 12) {
        savePreset();
        saveAnim();
        // page = keyboard_page;
      }

      break;

  }

  doChange = 1;

}

