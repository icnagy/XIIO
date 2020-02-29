
/* __/\\\_______/\\\____________/\\\\\\\\\\\____________/\\\\\\\\\\\_________________/\\\\\______
    _\///\\\___/\\\/____________\/////\\\///____________\/////\\\///________________/\\\///\\\____
     ___\///\\\\\\/__________________\/\\\___________________\/\\\_________________/\\\/__\///\\\__
      _____\//\\\\____________________\/\\\___________________\/\\\________________/\\\______\//\\\_
       ______\/\\\\____________________\/\\\___________________\/\\\_______________\/\\\_______\/\\\_
        ______/\\\\\\___________________\/\\\___________________\/\\\_______________\//\\\______/\\\__
         ____/\\\////\\\_________________\/\\\___________________\/\\\________________\///\\\__/\\\____
          __/\\\/___\///\\\____________/\\\\\\\\\\\____________/\\\\\\\\\\\______________\///\\\\\/_____
           _\///_______\///____________\///////////____________\///////////_________________\/////_______

  V3 by Csaba Nagy, 2020
  V2 by Máté Szabó, 2016
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
bool internalClockIsRunning = true;
uint32_t lastClockFallingEdge = 0;
uint8_t bpm = 60;

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

enum glideStatusEnum {
  GLIDE_OFF, // 0
  GLIDE_ON   // 1
};

enum glideDirectionEnum {
  GLIDE_DIRECTION_DOWN, // 0
  GLIDE_DIRECTION_UP    // 1
};

enum glideTimeEnum {
  GLIDE_TIME_THIRTYSECOND_NOTE,   // 0
  GLIDE_TIME_SIXTEENTH_NOTE,      // 1
  GLIDE_TIME_EIGHTH_NOTE,         // 2
  GLIDE_TIME_QUATER_NOTE,         // 3
  GLIDE_TIME_HALF_NOTE,           // 4
  GLIDE_TIME_FULL_NOTE            // 5
};

uint16_t glideStop = 0;  // the note to which we are gliding
uint16_t glideNote = 0;  // the current note, while gliding

glideDirectionEnum glideDirection = GLIDE_DIRECTION_DOWN;
glideStatusEnum gliding = GLIDE_OFF;      // true when gliding from a note to another is in progress
bool glideLegato = 0;  // true when in legato mode
glideTimeEnum glideTime = GLIDE_TIME_FULL_NOTE;
glideStatusEnum glideEnabled = GLIDE_OFF;
uint32_t glideCounter = 0;
int8_t newGlideTime = 0;

#define GLIDE_TIME_MULTIPLIER_FOR_FULL_NOTE         (1 << GLIDE_TIME_FULL_NOTE)
#define GLIDE_TIME_MULTIPLIER_FOR_HALF_NOTE         (1 << GLIDE_TIME_HALF_NOTE)
#define GLIDE_TIME_MULTIPLIER_FOR_QUATER_NOTE       (1 << GLIDE_TIME_QUATER_NOTE)
#define GLIDE_TIME_MULTIPLIER_FOR_EIGHTH_NOTE       (1 << GLIDE_TIME_EIGHTH_NOTE)
#define GLIDE_TIME_MULTIPLIER_FOR_SIXTEENTH_NOTE    (1 << GLIDE_TIME_SIXTEENTH_NOTE)
#define GLIDE_TIME_MULTIPLIER_FOR_THIRTYSECOND_NOTE (1 << GLIDE_TIME_THIRTYSECOND_NOTE)

const uint8_t GlideTimeMultiplier[] = {
  GLIDE_TIME_MULTIPLIER_FOR_THIRTYSECOND_NOTE,  // 0
  GLIDE_TIME_MULTIPLIER_FOR_SIXTEENTH_NOTE,     // 1
  GLIDE_TIME_MULTIPLIER_FOR_EIGHTH_NOTE,        // 2
  GLIDE_TIME_MULTIPLIER_FOR_QUATER_NOTE,        // 3
  GLIDE_TIME_MULTIPLIER_FOR_HALF_NOTE,          // 4
  GLIDE_TIME_MULTIPLIER_FOR_FULL_NOTE           // 5
};

uint16_t totalGlideTicks = 1000;
uint32_t glide_step_per_tick = 0;
uint32_t glide_accumlator = 0;

// INT1 times between 60 and 240 BPM
const uint16_t bpmTimeTable [] = {
  62499, 61474, 60482, 59522, 58592, 57691, 56817, 55969, 55146, 54346, 53570, 52815, 52082, 51368, 
  50674, 49999, 49341, 48700, 48075, 47467, 46874, 46295, 45730, 45179, 44641, 44116, 43603, 43102, 
  42612, 42133, 41665, 41207, 40759, 40321, 39892, 39472, 39061, 38658, 38264, 37877, 37499, 37127, 
  36763, 36406, 36056, 35713, 35376, 35045, 34721, 34402, 34089, 33782, 33481, 33184, 32893, 32607, 
  32326, 32050, 31778, 31511, 31249, 30990, 30736, 30486, 30240, 29998, 29760, 29526, 29295, 29068, 
  28845, 28624, 28408, 28194, 27984, 27776, 27572, 27371, 27172, 26977, 26784, 26594, 26407, 26222, 
  26040, 25861, 25683, 25509, 25336, 25166, 24999, 24833, 24670, 24508, 24349, 24192, 24037, 23884, 
  23733, 23583, 23436, 23290, 23147, 23005, 22864, 22726, 22589, 22454, 22320, 22188, 22057, 21928, 
  21801, 21675, 21550, 21427, 21305, 21185, 21066, 20948, 20832, 20717, 20603, 20490, 20379, 20269, 
  20160, 20052, 19945, 19840, 19735, 19632, 19530, 19429, 19328, 19229, 19131, 19034, 18938, 18843, 
  18749, 18655, 18563, 18471, 18381, 18291, 18202, 18114, 18027, 17941, 17856, 17771, 17687, 17604, 
  17522, 17440, 17360, 17280, 17200, 17122, 17044, 16967, 16890, 16815, 16740, 16665, 16591, 16518, 
  16446, 16374, 16303, 16232, 16162, 16093, 16024, 15956, 15888, 15821, 15755, 15689, 15624
};
// 1/32 note times in ticks on timer2 @ 1kHz for bpm range
const uint8_t _32note_ticks[] = {
  125, 123, 121, 119, 117, 115, 114, 112, 110, 109, 107, 106, 104, 103, 101, 100, 99, 97, 96, 95, 
  94, 93, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 82, 81, 80, 79, 78, 77, 77, 76, 75, 74, 74, 73, 
  72, 71, 71, 70, 69, 69, 68, 68, 67, 66, 66, 65, 65, 64, 64, 63, 63, 62, 61, 61, 60, 60, 60, 59, 
  59, 58, 58, 57, 57, 56, 56, 56, 55, 55, 54, 54, 54, 53, 53, 52, 52, 52, 51, 51, 51, 50, 50, 50, 
  49, 49, 49, 48, 48, 48, 47, 47, 47, 47, 46, 46, 46, 45, 45, 45, 45, 44, 44, 44, 44, 43, 43, 43, 
  43, 42, 42, 42, 42, 41, 41, 41, 41, 41, 40, 40, 40, 40, 39, 39, 39, 39, 39, 38, 38, 38, 38, 38, 
  38, 37, 37, 37, 37, 37, 36, 36, 36, 36, 36, 36, 35, 35, 35, 35, 35, 35, 34, 34, 34, 34, 34, 34, 
  33, 33, 33, 33, 33, 33, 33, 32, 32, 32, 32, 32, 32, 32, 32, 31, 31
};

uint16_t notes[] = {288, 300, 312, 324, 336, 348, 360, 372};

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
  Serial.begin(115200);
  Serial.println("XIIO v3");

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

  Serial.println("Pin setup complete");

  // clear eeprom at first startup
  if (EEPROM.read(1020) != 0) {
    Serial.println("Clearing EEPROM");
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

  Serial.println("FastLED initialized");

  SPI.begin();

  // initialize MPR121 library
  cap.begin(0x5A);
  //cap.setThreshholds(0x0F, 0x0A);
  cap.setThreshholds(3, 2);

  Serial.println("MPR121 initialized");

  totalGlideTicks = _32note_ticks[bpm] * GlideTimeMultiplier[glideTime];

  // recall last settings
  preset = EEPROM.read(1023);
  // loadPreset();

  encoderDirection = !EEPROM.read(1021);

  // enter calibration mode, if encoder is pushed while powering up
  if (switchRead == 0) {
    encoderDirection = !encoderDirection;
    EEPROM.update(1021, encoderDirection);
    switchLast = switchRead;
    switchHappened = 1;
  }

  Serial.println("Interrupt setup...");

  // TIMER 1 for interrupt frequency 2 Hz / 120 BPM:
  cli();                      // stop interrupts
  TCCR1A = 0;                 // set entire TCCR1A register to 0
  TCCR1B = 0;                 // same for TCCR1B
  TCNT1  = 0;                 // initialize counter value to 0
                              // set compare match register for bpm/60 Hz increments
  OCR1A = bpmTimeTable[bpm];  // = 16000000 / (256 * freq) - 1 (must be <65536)
  TCCR1B |= (1 << WGM12);     // turn on CTC mode
  TCCR1B |= (1 << CS12) | (0 << CS11) | (0 << CS10); // Set CS12, CS11 and CS10 bits for 256 prescaler                              
  TIMSK1 |= (1 << OCIE1A);    // enable timer compare interrupt

  // TIMER 2 for interrupt frequency 1000 Hz:
  TCCR2A = 0;                 // set entire TCCR2A register to 0
  TCCR2B = 0;                 // same for TCCR2B
  TCNT2  = 0;                 // initialize counter value to 0
                              // set compare match register for 1000 Hz increments
  OCR2A = 249;                // 1000 Hz (16000000/((124+1)*128))
  TCCR2A |= (1 << WGM21);     // turn on CTC mode
  TCCR2B |= (0 << CS22) | (1 << CS21) | (1 << CS20);  // Set CS22, CS21 and CS20 bits for 64 prescaler
  TIMSK2 |= (1 << OCIE2A);    // enable timer compare interrupt
  sei(); // allow interrupts
  Serial.println("Interrupt setup complete");
}

ISR(TIMER1_COMPA_vect) {
  newClock = 1;
  doTriggerFunction(1);
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
