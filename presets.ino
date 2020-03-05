/* things that has to be saved:

notes[0-7] = {288, 312, 480, 492, 516, 540, 564, 576};
min 288, max 708
after subtracting 288: min 0, max, 420

octaveBehavior             max value: 2  bits: 2
octaveAction               max value: 3  bits: 2
switchPlateBehavior[0]     max value: 2  bits: 2
switchPlateBehavior[1]     max value: 2  bits: 2

glideTime                  max value: 7  bits: 6
glideLegato                max value: 1  bits: 1
glideEnabled               max value: 1  bits: 1

arp                        max value: 1  bits: 1
enableFreeze               max value: 2  bits: 2
seqDirection               max value: 3  bits: 2

seqScope                   max value: 16 bits: 4
internalClockToggle        max value: 1  bits: 1
internalClockQuantTime     max value: 1  bits: 1
internalClockEmmitsTrigger max value: 1  bits: 1
euclidianSequencerRunning  max value: 1  bits: 1

internalClockBPMIndex      max value: 180 bits: 8

things, that has to be restored while a preset loads:

int16_t octave = 0;
uint16_t glideTime = 0;

*/

void savePreset(){

  uint16_t presetPrefix = preset * 15;
  uint8_t dataToStore = 0;

  // save notes

  for (int i = 0; i < 8; i ++){
    uint16_t noteToStore = notes[i] - 288;
    EEPROM.update(presetPrefix + i, noteToStore);
    bitWrite(dataToStore,i,bitRead(noteToStore,8)); // MSB of notes
  }

  EEPROM.update(presetPrefix + 8, dataToStore);

  // save octave and switch settings

  dataToStore = 0;

  dataToStore |= octaveBehavior;                // 0000 00XX
  dataToStore |= octaveAction << 2;             // 0000 XX00
  dataToStore |= switchPlateBehavior[0] << 4;   // 00XX 0000
  dataToStore |= switchPlateBehavior[1] << 6;   // XX00 0000

  EEPROM.update(presetPrefix + 9, dataToStore);

  // save glide settings

  dataToStore = 0;

  dataToStore |= glideTime;         // 00XX XXXX
  dataToStore |= glideLegato << 6;   // 0X00 0000
  dataToStore |= glideEnabled << 7;  // X000 0000

  EEPROM.update(presetPrefix + 10, dataToStore);

  // save auto settings

  dataToStore = 0;

  dataToStore |= mode;                 // 0000 00XX
  dataToStore |= enableFreeze << 2;    // 0000 0X00
  dataToStore |= seqDirection << 3;    // 000X X000
  dataToStore |= arpRandomScope << 5;  // XXX0 0000

  EEPROM.update(presetPrefix + 11, dataToStore);

  dataToStore = 0;

  dataToStore |= seqScope - 1;                          // 0000 XXXX
  dataToStore |= (internalClockToggle & 1) << 4;        // 000X 0000
  dataToStore |= (internalClockQuantTime & 1) << 5;     // 00X0 0000
  dataToStore |= (internalClockEmmitsTrigger & 1) << 6; // 0X00 0000
  dataToStore |= (euclidianSequencerRunning & 1) << 7;  // X000 0000

  EEPROM.update(presetPrefix + 12, dataToStore);

  EEPROM.update(presetPrefix + 13, (uint8_t)(internalClockBPMIndex & 0xff));
  // bytes (prefix+) 14 are reserved

  currentPreset = preset;
  EEPROM.update(1023, currentPreset);

}

void loadPreset(){

  uint16_t presetPrefix = preset * 15;
  uint8_t data = 0;

  // load notes

  data = EEPROM.read(presetPrefix + 8);

  for (int i = 0; i < 8; i ++){
    uint16_t loadNote = 0;
    loadNote = EEPROM.read(presetPrefix + i);
    bitWrite(loadNote,8,bitRead(data,i));
    loadNote += 288;
    if (loadNote > 708){
      loadNote = 708;
    }
    notes[i] = loadNote;
  }

  // load octave and switch settings

  data = EEPROM.read(presetPrefix + 9);

  octaveBehavior = data & B00000011;                  // 0000 00XX
  octaveAction  = (data >> 2) & B00000011;            // 0000 XX00
  switchPlateBehavior[0] = (data >> 4) & B00000011;   // 00XX 0000
  switchPlateBehavior[1] = (data >> 6) & B00000011;   // XX00 0000

  // load glide settings

  data = EEPROM.read(presetPrefix + 10);

  glideTime = data & B00111111;           // 00XX XXXX
  glideLegato = (data >> 6) & B00000001;   // 0X00 0000
  glideEnabled = (data >> 7) & B00000001;  // X000 0000

  // load arp settings

  data = EEPROM.read(presetPrefix + 11);

  mode = data & B00000011;                   // 0000 00XX
  enableFreeze = (data >> 2) & B00000001;    // 0000 0X00
  seqDirection = (data >> 3) & B00000011;    // 000X X000
  arpRandomScope = (data >> 5) & B00000111;  // XXX0 0000

  data = EEPROM.read(presetPrefix + 12);

  seqScope                   = (data & B00001111) + 1;  // 0000 XXXX
  internalClockToggle        = (data & B00010000) >> 4; // 000X 0000
  internalClockQuantTime     = (data & B00100000) >> 5; // 00X0 0000
  internalClockEmmitsTrigger = (data & B01000000) >> 6; // 0X00 0000
  internalClockBPMIndex = (uint8_t)EEPROM.read(presetPrefix + 13) & 0xff;
  // euclidianSequencerRunning = (data & B01000000) >> 7; // X000 0000
  if ((data & B01000000) >> 7) {
    euCalc(0);
    euCalc(1);
    euCalc(2);
    currPulse = 0;
    euclidianSequencerRunning = true;
  }
  // byte (prefix+) 14 is reserved

  if (glideEnabled){
    totalGlideTicks = _32noteTicks[internalClockBPMIndex] * GlideTimeMultiplier[glideTime];
  }
  else {
    gliding = GLIDE_OFF;
  }

  octave = 0;

  currentPreset = preset;
  EEPROM.update(1023, currentPreset);
}

void loadAnim() {
  uint8_t dtime = 40;

  for (int i = 0; i < 4; i++) {
    leds[i] = LED_GREEN;
    leds[i].nscale8(brightness);
    FastLED.show();
    delay(dtime);
  }

  for (int i = 0; i < 4; i++) {
    leds[i] = LED_GREEN_LOW;
    leds[i].nscale8(brightness);
    FastLED.show();
    delay(dtime);
  }

  for (int i = 0; i < 4; i++) {
    leds[i] = 0x0;
    leds[i].nscale8(brightness);
    FastLED.show();
    delay(dtime);
  }

}

void saveAnim() {
  uint8_t dtime = 40;

  for (int i = 0; i < 4; i++) {
    leds[i] = LED_RED;
    leds[i].nscale8(brightness);
    FastLED.show();
    delay(dtime);
  }

  for (int i = 0; i < 4; i++) {
    leds[i] = LED_RED_LOW;
    leds[i].nscale8(brightness);
    FastLED.show();
    delay(dtime);
  }

  for (int i = 0; i < 4; i++) {
    leds[i] = 0x0;
    leds[i].nscale8(brightness);
    FastLED.show();
    delay(dtime);
  }

}

