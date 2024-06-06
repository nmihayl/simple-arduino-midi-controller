#include <MIDI.h>
#include <Keypad.h>

MIDI_CREATE_DEFAULT_INSTANCE();

// Define pins and vars
// Joystick
const int joyXPin = A0;
const int joyYPin = A1;
const int joyButtonPin = 0;
int lastXValue = 0;
int lastYValue = 0;

// Knobs
const int knob1 = A4;
int knob1Value = 0;
int lastKnob1Value = 0;
const int knob2 = A5;
int knob2Value = 0;
int lastKnob2Value = 0;

// Keypad
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {13, 12, 11, 10}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 8, 7, 6}; // Connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Define MIDI CC mapping
const int midiChannel = 1;
const int ccNumberX = 1; // Control Change number for X axis of joystick
const int ccNumberY = 74; // Control Change number for Y axis of joystick
const int ccKnob1 = 73; // Control Change for knob 1
const int ccKnob2 = 9; // Control Change for knob 2

// Main program
void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  Serial.begin(9600);
  pinMode(joyButtonPin, INPUT_PULLUP);
  keypad.setDebounceTime(5);
}


void loop() {
  // Joystick
  int xValue = analogRead(joyXPin) >> 3; // Scale 0-1023 to 0-127 using bit shift
  int yValue = analogRead(joyYPin) >> 3; // Scale 0-1023 to 0-127 using bit shift

  if (xValue != lastXValue) {
    MIDI.sendControlChange(ccNumberX, xValue, midiChannel);
    lastXValue = xValue;
  }

  if (yValue != lastYValue) {
    MIDI.sendControlChange(ccNumberY, yValue, midiChannel);
    lastYValue = yValue;
  }

  // Keypad handling
  char key = keypad.getKey();
  if (key) {
    int note = -1;
    switch (key) {
      case '1': note = 36; break; // C1
      case '2': note = 38; break; // D1
      case '3': note = 40; break; // E1
      case 'A': note = 41; break; // F1
      case '4': note = 43; break; // G1
      case '5': note = 45; break; // A1
      case '6': note = 47; break; // B1
      case 'B': note = 48; break; // C2
      case '7': note = 50; break; // D2
      case '8': note = 52; break; // E2
      case '9': note = 53; break; // F2
      case 'C': note = 55; break; // G2
      case '*': note = 57; break; // A2
      case '0': note = 59; break; // B2
      case '#': note = 60; break; // C3
      case 'D': note = 62; break; // D3
    }
    if (note != -1) {
      MIDI.sendNoteOn(note, 127, midiChannel);
      MIDI.sendNoteOff(note, 0, midiChannel);
    }
  }

  // Joystick button handling
  if (digitalRead(joyButtonPin) == LOW) {
    // Add code here if you want to handle the joystick button press
  }

  // Knob 1 handling
  knob1Value = analogRead(knob1);
  int knob1ValueMap = map(knob1Value, 0, 1023, 1, 127);
  if (knob1ValueMap != lastKnob1Value) {
    MIDI.sendControlChange(ccKnob1, knob1ValueMap, midiChannel);
    lastKnob1Value = knob1ValueMap;
  }

  // Knob 2 handling
  knob2Value = analogRead(knob2);
  int knob2ValueMap = map(knob2Value, 0, 1023, 1, 127);
  if (knob2ValueMap != lastKnob2Value) {
    MIDI.sendControlChange(ccKnob2, knob2ValueMap, midiChannel);
    lastKnob2Value = knob2ValueMap;
  }

  // Serial monitor output
  Serial.print("X: "); Serial.print(xValue); Serial.print(" Y: "); Serial.print(yValue); Serial.print(" Key: "); Serial.print(key); Serial.print(" Knob 1: "); Serial.print(knob1ValueMap); Serial.print(" Knob 2: "); Serial.println(knob2ValueMap);
}
