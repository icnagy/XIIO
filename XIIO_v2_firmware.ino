
/* __/\\\_______/\\\____________/\\\\\\\\\\\____________/\\\\\\\\\\\_________________/\\\\\______
    _\///\\\___/\\\/____________\/////\\\///____________\/////\\\///________________/\\\///\\\____
     ___\///\\\\\\/__________________\/\\\___________________\/\\\_________________/\\\/__\///\\\__
      _____\//\\\\____________________\/\\\___________________\/\\\________________/\\\______\//\\\_
       ______\/\\\\____________________\/\\\___________________\/\\\_______________\/\\\_______\/\\\_
        ______/\\\\\\___________________\/\\\___________________\/\\\_______________\//\\\______/\\\__
         ____/\\\////\\\_________________\/\\\___________________\/\\\________________\///\\\__/\\\____
          __/\\\/___\///\\\____________/\\\\\\\\\\\____________/\\\\\\\\\\\______________\///\\\\\/_____
           _\///_______\///____________\///////////____________\///////////_________________\/////_______

  by Máté Szabó, 2016
  www.doboz.audio

*/



// FastLED library
#include <FastLED.h>
#define NUM_LEDS 4
#define DATA_PIN 9
#define LED_BLACK      0x000000  // R:  0 G:  0 B:  0
#define LED_WHITE      0x282828  // R: 40 G: 40 B: 40
#define LED_WHITE_LOW  0x090909  // R:  9 G:  9 B:  9
#define LED_WHITE_BLU  0x09000B  // R:  0 G:  9 B: 11
#define LED_RED        0x003700  // R: 55 G:  0 B:  0
#define LED_RED_LOW    0x000900  // R:  9 G:  0 B:  0
#define LED_GREEN      0x320911  // R:  9 G: 50 B: 17
#define LED_GREEN_LOW  0x140509  // R:  5 G: 20 B:  9
#define LED_BLUE       0x00003C  // R:  0 G:  0 B: 60
#define LED_BLUE_LOW   0x070010  // R:  0 G:  7 B: 16
#define LED_PURPLE     0x001E1E  // R: 30 G:  0 B: 30
#define LED_PURPLE_LOW 0x000C0A  // R: 12 G:  0 B: 10
#define LED_YELLOW     0x142800  // R: 40 G: 20 B:  0
#define LED_YELLOW_LOW 0x0A0F00  // R: 15 G: 10 B:  0
CRGB leds[NUM_LEDS];
uint8_t brightness = 40;
uint8_t R  = B0000;
uint8_t RL = B0000;
uint8_t G  = B0000;
uint8_t GL = B0000;
uint8_t B  = B0000;
uint8_t BL = B0000;
uint8_t Y  = B0000;
uint8_t YL = B0000;
uint8_t P  = B0000;
uint8_t PL = B0000;
uint8_t W  = B0000;
uint8_t WL = B0000;
uint8_t WB = B0000;

// EEPROM library
#include <EEPROM.h>

// DAC controlled via SPI
#include <SPI.h>
#include <Wire.h>

// MPR121 library
#include "Adafruit_MPR121.h"
Adafruit_MPR121 cap = Adafruit_MPR121();



// encoder
bool encoderDirection = 0;
#define encAPin 2 // PD2
#define encBPin 3 // PD3
#define encARead (PIND & B00000100)
#define encBRead (PIND & B00001000)

uint8_t lastEnc = 0;
uint8_t readEnc = 0;

#define switchPin 4 // PD4
#define switchRead (PIND & B00010000)
bool switchState = 0;
uint32_t switchTime = 0;
uint32_t switchDebounce = 0;
bool switchHappened = 0;
bool switchLast = 1;
uint8_t switchAction = 0;
uint8_t switchActionLast = 0;
#define shortpush 1
#define longpush 2

// gate
#define gatePin 7 // PD7
#define gateHigh PORTD &= ~_BV(PD7)
#define gateLow PORTD |= _BV(PD7)
bool gateState = 0;
bool gateStateLast = 0;

// trigger
#define triggerPin 5 // PD5
#define triggerHigh PORTD &= ~_BV(PD5)
#define triggerLow PORTD |= _BV(PD5)
bool triggerState = 0;
uint32_t triggerTime = 0;

// switch 0
#define switch0pin 8 // PB0
#define switch0high PORTB &= ~_BV(PB0)
#define switch0low PORTB |= _BV(PB0)

// switch 1
#define switch1pin 6 // PD6
#define switch1high PORTD &= ~_BV(PD6)
#define switch1low PORTD |= _BV(PD6)

// clock input
#define clockPin A3 // PC3
#define readClockPin (PINC & B00001000)
bool readClock = 1;
bool lastClock = 1;
bool newClock = 0;
// Internal clock
uint8_t bpm = 120;
uint8_t interrup_c = 129;



uint8_t page = 0;
#define keyboard_page 0
#define settings_page 1
#define function_page 3


bool doChange = 0;
bool doNote = 0;
bool doTrigger = 0;

uint16_t plates;
uint16_t platesLast;
uint8_t activePlate;

uint8_t notePlates;
uint8_t notePlatesLast;
uint8_t activeNotePlates;
uint8_t activeNote;

uint8_t octaveBehavior = 0;
uint8_t octaveAction = 0;
#define queue 0
#define immediate_notrig 1
#define immediate_onetrig 2
#define immediate_twotrig 3
int8_t octaveRead = 0;
int8_t octaveReadLast = 0;
int16_t octave = 0;
int16_t queuedOctave = 0;


uint8_t switchPlateBehavior [] = {0, 0};
#define momentary_switch 0
#define trigger_switch 1
#define latching_switch 2
bool switchPlateRead [] = {0, 0};
bool switchPlateLast [] = {0, 0};
bool switchPlateStatus [] = {0, 0};
uint32_t switchPlateTriggerTime [] = {0, 0};

uint16_t glideStop = 0;  // the note to which we are gliding
uint16_t glideNote = 0;  // the current note, while gliding
bool glideDirection = 0;
uint16_t glideLast = 0;  // the last time, when DAC was refreshed

bool gliding = 0;      // true when gliding from a note to another is in progress
bool glideLegato = 0;  // true when in legato mode
uint8_t glideIndex = 0;

const uint16_t glideTimeTable [] = {
  15, 21, 26, 31, 36, 40, 45, 52, 61, 69, 78, 87, 104, 122, 139, 156,
  174, 191, 208, 226, 243, 260, 313, 365, 417, 469, 521, 608, 694, 781, 868, 955,
  1042, 1128, 1302, 1389, 1389, 1736, 1910, 2083, 2257, 2431, 2604, 2778, 2951, 3125, 3299, 3472
};


uint16_t notes[] = {288, 288, 288, 288, 288, 288, 288, 288};

uint8_t mode = 0;
#define keyboard 0
#define arpeggiator 1
#define sequencer 2
uint8_t seqLength = 0; // determines the length of sequence in arp mode
int8_t seqIndex = 0;
uint8_t seqNotes [] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t arpGlide = 0;

uint8_t enableFreeze = 0;
uint8_t freeze = 0;

uint8_t seqDirection = 0;
#define dir_up 0
#define dir_down 1
#define dir_pingpong 2
#define dir_random 3

bool pingpongDirection = 0;
// 0 - down
// 1 - up

bool allowRandom = 0;

uint8_t arpRandomScope = 0;
// 0 > off
// random octave calculated at the end of array
// 1 > +1 oct
// 2 > +2 oct
// 3 > -1/+1 oct
// random octave calculated at each new note
// 4 > +1 oct
// 5 > +2 oct
// 6 > -1/+1 oct
int8_t randomOct = 0;
int16_t randomOctValue = 0;
bool doRandomOct = 0;

uint8_t seqScope = 1; // determines the length of sequence in seq mode
int16_t seqOctaves [] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bool moveSeq = 0;

uint8_t preset = 0;
uint8_t currentPreset;

bool blinkStatus;
uint32_t blinkTime;


void setup() {

  // encoder
  pinMode(switchPin, INPUT);
  digitalWrite(switchPin, HIGH);
  pinMode(encAPin, INPUT);
  digitalWrite(encAPin, HIGH);
  pinMode(encBPin, INPUT);
  digitalWrite(encBPin, HIGH);

  pinMode(gatePin, OUTPUT);
  digitalWrite(gatePin, HIGH);
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, HIGH);
  pinMode(switch0pin, OUTPUT);
  digitalWrite(switch0pin, HIGH);
  pinMode(switch1pin, OUTPUT);
  digitalWrite(switch1pin, HIGH);

  pinMode(clockPin, INPUT);
  digitalWrite(clockPin, HIGH);

  pinMode(9, OUTPUT); // LED data output
  pinMode(10, OUTPUT); // DAC CS pin
  digitalWrite(10, HIGH);

  // clear eeprom at first startup
  if (EEPROM.read(1020) != 0) {
    for (int i = 0; i < 1021; i ++) {
      EEPROM.update(i, 0);
    }
    EEPROM.update(1023, 0); // current preset
    EEPROM.update(1022, 50); // LED brightness
    EEPROM.update(1021, 0); // encoder direction
  }

  // initialize the FastLED library
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  leds[0] = 0;
  leds[1] = 0;
  leds[2] = 0;
  leds[3] = 0;
  FastLED.show();
  brightness = EEPROM.read(1022);
  ahoj();

  SPI.begin();

  // initialize MPR121 library
  cap.begin(0x5A);
  //cap.setThreshholds(0x0F, 0x0A);
  cap.setThreshholds(3, 2);

  xiioBegin();

  // recall last settings
  preset = EEPROM.read(1023);
  loadPreset();

  encoderDirection = EEPROM.read(1021);

  // enter calibration mode, if encoder is pushed while powering up
  if (switchRead == 0) {
    encoderDirection = !encoderDirection;
    EEPROM.update(1021, encoderDirection);
    switchLast = switchRead;
    switchHappened = 1;
  }

  interrup_c = 16000000 / (1024 * bpm) - 1;

  // TIMER 0 for interrupt frequency 190.5487804878049 Hz:
  cli(); // stop interrupts
  TCCR0A = 0; // set entire TCCR0A register to 0
  TCCR0B = 0; // same for TCCR0B
  TCNT0  = 0; // initialize counter value to 0
  // set compare match register for 190.5487804878049 Hz increments
  OCR0A = interrup_c; // = 16000000 / (1024 * bpm) - 1 (must be <256)
  // turn on CTC mode
  TCCR0B |= (1 << WGM01);
  // Set CS02, CS01 and CS00 bits for 1024 prescaler
  TCCR0B |= (1 << CS02) | (0 << CS01) | (1 << CS00);
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);
  sei(); // allow interrupts

}

ISR(TIMER1_COMPA_vect) {

  // calculate new gliding note
  glideNote = glideNote - 1 + (glideDirection * 2);

  // do we stop gliding?
  if (glideNote == glideStop || glideNote > 996) {
    glideNote = glideStop;
    // if yes, switch of interrupts
    TIMSK1 = 0x0;
    gliding = 0;
  }

  // refresh DAC
  writeDAC(glideNote);
}

void loop() {

  doChange = 0;
  doNote = 0;
  getMPR121();
  encoder();
  switchPlates();
  switch (mode) {
    case keyboard:
    break;
    case arpeggiator:
    getClock();
    arpFunction();
    break;
    case sequencer:
    getClock();
    seqFunction();
    break;
  }

  // turn of trigger output
  if (triggerState && millis() - triggerTime >= 10) { // trigger width = 5mS
    doTriggerFunction(0);

  }
  // blinking
  if (page == settings_page && millis() - blinkTime > 100) {
    blinkStatus = !blinkStatus;
    blinkTime = millis();
    doChange = 1;
  }


  // refresh LEDs if change happened
  if (doChange) {
    doLeds();
  }

}

void xiioBegin() {
  // timer1 settings, used when gliding
  TCCR1A = 0x0;
  TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);    // mode: CTC, prescaler: 64
  OCR1A = 100;
  TIMSK1 = 0x0;
}

void ahoj() {
  uint8_t dtime = 40;

  leds[3] = 0xFF;
  leds[2] = 0xFF;
  leds[1] = 0xFF;
  leds[0] = 0xFF;

  for (int i = 0; i < 4; i++) {
    leds[i] = LED_BLUE;
    FastLED.show();
    delay(dtime);
  }

  for (int i = 0; i < 4; i++) {
    leds[i] = LED_BLUE_LOW;
    FastLED.show();
    delay(dtime);
  }

  for (int i = 0; i < 4; i++) {
    leds[i] = 0x0;
    FastLED.show();
    delay(dtime);
  }

}
