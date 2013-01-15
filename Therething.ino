#define SENSOR_HC true
//#define SENSOR_IR true
#define DEBUG true
#define LCD_ROWS 2

#if SENSOR_HC
#include <NewPing.h>

#define TRIGGER_PIN_L  A0
#define ECHO_PIN_L     A1
#define TRIGGER_PIN_R A2
#define ECHO_PIN_R     A3

#define MAX_DISTANCE_CM 120
NewPing ultrasoundL(TRIGGER_PIN_L, ECHO_PIN_L, MAX_DISTANCE_CM);
NewPing ultrasoundR(TRIGGER_PIN_R, ECHO_PIN_R, MAX_DISTANCE_CM);
#endif

#include <avr/pgmspace.h>
#include <LiquidCrystal.h>

#include <EEPROM.h>


#define MENU_TIMEOUT 5000 // Milliseconds until the menu timesout

#ifdef SENSOR_IR
#define SENSOR_MIN 100
#define SENSOR_MAX 500 // Range of valid sensor readings
#endif

#if SENSOR_HC
#define SENSOR_MIN 150 // Lowest valid sensor reading
#define SENSOR_MAX 5000 // Range of valid sensor readings
#endif

#define SENSOR_RANGE SENSOR_MAX - SENSOR_MIN

#define SENSOR_SCALE_FACTOR (127.0 / (float)SENSOR_RANGE)
#define SCALE_LENGTH 7

#define LED_MAX 255

//////////////////////////////////////////////////////////////////////////////
// Pin configuration

enum encoder_pins {
  ENCODER_A = 3,
  ENCODER_B = 4,
  ENCODER_CLICK = 2
};

enum lcd_pins {
  LCD_RS = 11,
  LCD_RW = 10,
  LCD_ENABLE = 9,
  LCD_D4 = 8,
  LCD_D5 = 7,
  LCD_D6 = 6,
  LCD_D7 = 5
};

enum mode {
  CONTROLLER,
  NOTES
};

enum midi{
 NOTE_OFF =  0,
 NOTE_ON = 1, 
 CC = 3,
 PITCH_BEND = 6,
};

enum led_pins {
  NOTE_LED = 18,
  CONTROLLER_LED = 19
};

char scales[20][7] = { 
  { 2,2,1,2,2,2,1 }, // Major
  { 2,1,2,2,1,2,2 }, // Minor
  { 2,2,1,2,2,2,1 }, // Ionian Mode
  { 2,1,2,2,2,1,2 }, // Dorian Mode
  { 1,2,2,2,1,2,2 }, // Phrygian Mode
  { 2,2,2,1,2,2,1 }, // Lydian Mode
  { 2,2,1,2,2,1,2 }, // Mixolydian
  { 2,1,2,2,1,2,2 }, // Aeolian Mode
  { 1,2,2,1,2,2,2 }, // Locrian Mode
  { 2,2,1,2,2,2,1 }, // Major Scale
  { 2,1,2,2,1,3,1 }, // Harmonic Minor
  { 2,1,2,2,2,2,1 }, // Asc. Mel Minor
  { 2,1,2,2,1,2,2 }, // Des. Mel Minor
  { 2,1,2,2,1,2,2 }, // Natural Minor
  { 2,1,3,1,2,2,1 }, // Lydian Dim
  { 2,2,2,2,1,2,1 }, // Lydian Aug
  { 2,2,2,1,2,1,2 }, // Lydian b7
  { 2,1,2,1,2,2,2 }, // Locrian #2
  { 1,2,1,2,2,2,2 }, // Super Locrian
  { 2,2,2,2,2,2,2 }  // Whole Tone
};

//////////////////////////////////////////////////////////////////////////////
// Menus

prog_char menu__back[] PROGMEM = "< Back";

prog_char top_menu__note_settings[] PROGMEM = "Note Settings";
prog_char top_menu__controller_settings[] PROGMEM = "Controller Settings";
prog_char top_menu__midi_channel[] PROGMEM = "MIDI Channel";

prog_char controller_menu__inv_left_on[] PROGMEM  = "Inv Left [On]";
prog_char controller_menu__inv_left_off[] PROGMEM = "Inv Left [Off]";
prog_char controller_menu__inv_right_on[] PROGMEM  = "Inv Right [On]";
prog_char controller_menu__inv_right_off[] PROGMEM = "Inv Right [Off]";

prog_char* top_menu[] = {
  top_menu__note_settings,
  top_menu__controller_settings,
  top_menu__midi_channel,
  controller_menu__inv_left_off,
  controller_menu__inv_right_off,
  NULL
};

prog_char note_menu__scale[] PROGMEM = "Scale";
prog_char note_menu__root[] PROGMEM = "Root";
prog_char note_menu__octave[] PROGMEM = "Octave";
prog_char note_menu__range[] PROGMEM = "Range";

prog_char* note_menu[] = {
  note_menu__scale,
  note_menu__root,
  note_menu__octave,
  note_menu__range,
  menu__back,
  NULL
};

prog_char controller_menu__left_cc_number[] PROGMEM = "Left CC Number";
prog_char controller_menu__right_cc_number[] PROGMEM = "Right CC Number";

prog_char* controller_menu[] = {
  controller_menu__left_cc_number,
  controller_menu__right_cc_number,
  menu__back,
  NULL
};

prog_char scale_menu__major[] PROGMEM = "Major";
prog_char scale_menu__minor[] PROGMEM = "Minor";
prog_char scale_menu__ionian_mode[] PROGMEM = "Ionian Mode";
prog_char scale_menu__dorian_mode[] PROGMEM = "Dorian Mode";
prog_char scale_menu__phrygian_mode[] PROGMEM = "Phrygian Mode ";
prog_char scale_menu__lydian_mode[] PROGMEM = "Lydian Mode";
prog_char scale_menu__mixolydian[] PROGMEM = "Mixolydian";
prog_char scale_menu__aeolian_mode[] PROGMEM = "Aeolian Mode";
prog_char scale_menu__locrian_mode[] PROGMEM = "Locrian Mode";
prog_char scale_menu__major_scale[] PROGMEM = "Major Scale";
prog_char scale_menu__harmonic_minor[] PROGMEM = "Harmonic Minor";
prog_char scale_menu__asc_mel_minor[] PROGMEM = "Asc. Mel Minor";
prog_char scale_menu__des_mel_minor[] PROGMEM = "Des. Mel Minor";
prog_char scale_menu__natural_minor[] PROGMEM = "Natural Minor";
prog_char scale_menu__lydian_dim[] PROGMEM = "Lydian Dim";
prog_char scale_menu__lydian_aug[] PROGMEM = "Lydian Aug";
prog_char scale_menu__lydian_b7[] PROGMEM = "Lydian b7";
prog_char scale_menu__locrian_2[] PROGMEM = "Locrian #2";
prog_char scale_menu__super_locrian[] PROGMEM = "Super Locrian";
prog_char scale_menu__whole_tone[] PROGMEM = "Whole Tone";

prog_char* scale_menu[] = {
  scale_menu__major,
  scale_menu__minor,
  scale_menu__ionian_mode,
  scale_menu__dorian_mode,
  scale_menu__phrygian_mode,
  scale_menu__lydian_mode,
  scale_menu__mixolydian,
  scale_menu__aeolian_mode,
  scale_menu__locrian_mode,
  scale_menu__major_scale,
  scale_menu__harmonic_minor,
  scale_menu__asc_mel_minor,
  scale_menu__des_mel_minor,
  scale_menu__natural_minor,
  scale_menu__lydian_dim,
  scale_menu__lydian_aug,
  scale_menu__lydian_b7,
  scale_menu__locrian_2,
  scale_menu__super_locrian,
  scale_menu__whole_tone,
  NULL
};

prog_char root_menu__c[] PROGMEM = "C";
prog_char root_menu__c_[] PROGMEM = "C#";
prog_char root_menu__d[] PROGMEM = "D";
prog_char root_menu__d_[] PROGMEM = "D#";
prog_char root_menu__e[] PROGMEM = "E";
prog_char root_menu__f[] PROGMEM = "F";
prog_char root_menu__f_[] PROGMEM = "F#";
prog_char root_menu__g[] PROGMEM = "G";
prog_char root_menu__g_[] PROGMEM = "G#";
prog_char root_menu__a[] PROGMEM = "A";
prog_char root_menu__a_[] PROGMEM = "A#";
prog_char root_menu__b[] PROGMEM = "B";

prog_char* root_menu[] = {
  root_menu__c,
  root_menu__c_,
  root_menu__d,
  root_menu__d_,
  root_menu__e,
  root_menu__f,
  root_menu__f_,
  root_menu__g,
  root_menu__g_,
  root_menu__a,
  root_menu__a_,
  root_menu__b,
  NULL
};

prog_char octave_menu__minus_2[] PROGMEM = "-2";
prog_char octave_menu__minus_1[] PROGMEM = "-1";
prog_char octave_menu__0[] PROGMEM = "0";
prog_char octave_menu__1[] PROGMEM = "1";
prog_char octave_menu__2[] PROGMEM = "2";
prog_char octave_menu__3[] PROGMEM = "3";

prog_char* octave_menu[] = {
  octave_menu__minus_2,
  octave_menu__minus_1,
  octave_menu__0,
  octave_menu__1,
  octave_menu__2,
  NULL
};

prog_char range_menu__1_octave[] PROGMEM = "1 octave";
prog_char range_menu__2_octaves[] PROGMEM = "2 octaves";
prog_char range_menu__3_octaves[] PROGMEM = "3 octaves";
prog_char range_menu__4_octaves[] PROGMEM = "4 octaves";

prog_char* range_menu[] = {
  range_menu__1_octave,
  range_menu__2_octaves,
  range_menu__3_octaves,
  range_menu__4_octaves,
  NULL
};

enum {
  MAIN_MENUS,
  LEFT_CC_NO,
  RIGHT_CC_NO,
  MIDI_CHANNEL
} menu_area = MAIN_MENUS;

enum {
  MENU,
  MUSIC
} last_loop = MENU;


//////////////////////////////////////////////////////////////////////////////
// Transient state, variables and buffers

prog_char** menu; // The currently active menu

volatile int item = 0; // The currently selected item
volatile int last_item = !item; // The item selected on last display update

char buffer[21]; // String buffer used to copy string constants out of flash

unsigned long last_interrupt = 0; // Last time the rotary encoder was used

mode optionMode;

//Set to true temporarily to flag that UI should show current mode.
boolean modeChanged = false;

unsigned char currentNotes[50]; // The current notes to play, should be mapped to the entire sensing length

float bucketSize; // sensor range  / number of notes. Will be set by makeScale()


//////////////////////////////////////////////////////////////////////////////
// Persistant state
// These are restored from eeprom
char scale; // The current scale. Index into scale array
char root; // The root of the scale. Index into C,C#,D,D#,E,F,F#,G,G#,A,A#,B
char octave; // Which octave is at the bottom end of the sensor range
char range; // The number of octaves covered by the sensor
char left_cc_number; // The MIDI CC Number of the left sensor
char right_cc_number; // The MIDI CC NUmber of the right sensor
char midi_channel; // The MIDI channel number
boolean inv_left; // When true the left sensor range is inverted
boolean inv_right; // When true the right sensor range is inverted


//////////////////////////////////////////////////////////////////////////////
// Devices

LiquidCrystal lcd(LCD_RS, LCD_RW, LCD_ENABLE, LCD_D4,LCD_D5,LCD_D6,LCD_D7);

//////////////////////////////////////////////////////////////////////////////
// Called by the Arduino firmware just after reset.
// Sets up the IO pins, attaches the interrupt service routines, configures
// the devices and sets up the persistant and transient state variables.

void setup() {
  // Persistant state
  scale = eepromGet(0, 0);
  root = eepromGet(1, 0);
  octave = eepromGet(2, 2);
  range = eepromGet(3, 2);
  left_cc_number = eepromGet(4, 2);
  right_cc_number = eepromGet(5, 3);
  midi_channel = eepromGet(6, 0);
  inv_left = eepromGet(7, 0) && 1;
  inv_right = eepromGet(8, 0) && 1;
  optionMode = eepromGet(9, 1) == 0 ? NOTES : CONTROLLER;
  if(inv_left){
    top_menu[3] = controller_menu__inv_left_on;
  }else{
    top_menu[3] = controller_menu__inv_left_off;
  }
  if(inv_right){
    top_menu[4] = controller_menu__inv_right_on; 
  }else{
    top_menu[4] = controller_menu__inv_right_off;
  } 

  // Transient state
  menu = top_menu;
  item = 0;
  last_item = -1;

  // Pin setup and ISR for the rotary encoder's click function
  pinMode(ENCODER_CLICK, INPUT);
  digitalWrite(ENCODER_CLICK, HIGH);
  attachInterrupt(0, click, FALLING);
  
  // Pin setup and ISR for the rotary encoder's rotate function
  pinMode(ENCODER_A, INPUT);
  pinMode(ENCODER_B, INPUT);
  digitalWrite(ENCODER_A, HIGH);
  digitalWrite(ENCODER_B, HIGH);
  attachInterrupt(1, turn, RISING);
    
  // Initialise the LCD
  lcd.begin(20,LCD_ROWS);

#if DEBUG
  // debug rate
  Serial.begin(115200);
#else
  // Set MIDI baud rate
  Serial.begin(31250);
#endif
  
  makeScale();
  
  pinMode(NOTE_LED, OUTPUT);
  pinMode(CONTROLLER_LED, OUTPUT);
  digitalWrite(NOTE_LED, optionMode == NOTES ? HIGH : LOW);
  digitalWrite(CONTROLLER_LED, optionMode == CONTROLLER ? HIGH : LOW);

}

byte eepromGet(int location, byte fallback){
  //ASSUMPTION CITY: if value is 255, it's not been written to yet, mm k?
  byte value = EEPROM.read(location);
  if(value == 255){
    return fallback;
  }else{
    return value;
  }
}


//////////////////////////////////////////////////////////////////////////////
// Main program loop. Called repeatedly by the Arduino firmware

void loop(){
  if (last_interrupt > (millis() - MENU_TIMEOUT)) {
    doMenu();
    last_loop = MENU;
  } else {
    if(last_loop != MUSIC){
      lcd.clear();
    }
    doMusic();
    last_loop = MUSIC;
  }

}

//////////////////////////////////////////////////////////////////////////////
// Retreive a measurement from the passed ultrasound

#if SENSOR_HC
int getMeasurement(NewPing s)
{
    unsigned long raw = s.ping();
#if DEBUG
    Serial.print("RAW ");
    Serial.println(raw);
#endif
    return max(0, min(s.ping() - SENSOR_MIN, SENSOR_MAX));
}
#endif

int getIrMeasurement(int pin){
  int value = analogRead(pin);
  value = max(0, min(value-SENSOR_MIN, SENSOR_MAX));
  return value;
}


//////////////////////////////////////////////////////////////////////////////
// Calculate and return the number of items in the current menu

int totalItems() {
  int total_items = 0;
  prog_char** m = menu;
  while (*(m++)) total_items++;
  return total_items;
}

int getNoteFromScale(int sensorReading){
  int bucket = floor(sensorReading / bucketSize);
  int note = currentNotes[bucket];
  return note;
}

/* Fills in currentNotes with the midi notes to play
        and bucketSize with the size of the sections each note will play for along the sensor range */
void makeScale(){
  // Add one to the length so we go all the way back to the root note again.
  int notesSize = (range * SCALE_LENGTH) + 1;
  bucketSize = (float)(SENSOR_RANGE + 1) / (float)notesSize;
  
  int transposition = (octave * 12) + 12;
  // Set transposition up so that optionStartOctave of 0 gets us to note 12
  currentNotes[0] = root + transposition;

  for (int note = 0; note < (notesSize - 1); note++) {
    currentNotes[note + 1] = currentNotes[note] + scales[scale][note % SCALE_LENGTH];
  }
}

int prevNote = 0;
int prevVel = 0;

void sendNote(int note, int vel){
  int noteNumber = getNoteFromScale(note);
  lcd.setCursor(0,0);
  lcd.print("Note: ");
  lcd.print(getNoteName(noteNumber));
  lcd.print(getOctaveNumber(noteNumber));
  lcd.print("    ");
  //Scale velocity over entire sensor range
  int scaledVel = (float)vel * SENSOR_SCALE_FACTOR;
  lcd.setCursor(0,LCD_ROWS / 2);
  lcd.print("Vel:  ");
  lcd.print(scaledVel);
  lcd.print("    ");
  
  if(noteNumber != prevNote){
    /* next note on, then previous note off - 
       this is so we can overlap the notes and glide between them if needed.
    */
    noteOn(noteNumber, scaledVel);
    noteOff();
    //save the inputs so we can turn the 
    //note off when the next ones turned on
    prevNote = noteNumber;
    prevVel = scaledVel;
  }else if(scaledVel == 0 && prevVel != 0){
    noteOff();
    prevVel = 0;
  }
}

/* Note name functions */
char* noteNames[] = {
  "C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};

char* getNoteName(int noteNumber){
  return noteNames[noteNumber % 12];
}

int getOctaveNumber(int noteNumber){
  return (noteNumber / 12) - 1;
}

/* MIDI Functions */

//Sends a note on event over serial
void noteOn(unsigned char noteNo, unsigned char vel){
  sendMidi(NOTE_ON, noteNo, vel);
}

//Sends a note on event over serial
void noteOff(){
  sendMidi(NOTE_OFF, prevNote, prevVel);
}

int prevCont1 = 256;
int prevCont2 = 256;

void sendControllers(int c1, int c2){
  
#if DEBUG
  Serial.print("\t\t\t");
  Serial.print(c1);
  Serial.print("\t");
  Serial.println(c2);
#endif

  //Scale CCs over entire sensor range
  int controller1 = (float)c1 * SENSOR_SCALE_FACTOR;
  int controller2 = (float)c2 * SENSOR_SCALE_FACTOR;
  
  if(controller1 != prevCont1){
    prevCont1 = controller1;
    sendCC(left_cc_number, controller1);
  }
  lcd.setCursor(0,0);
  lcd.print("CC# ");
  lcd.print((int)left_cc_number);
  lcd.setCursor(7,0);
  lcd.print(": ");
  lcd.print(controller1);
  lcd.print("  ");
  
  if(controller2 != prevCont2){
    prevCont2 = controller2;
    sendCC(right_cc_number, controller2);
  }
  lcd.setCursor(0, LCD_ROWS / 2);
  lcd.print("CC# ");
  lcd.print((int)right_cc_number);
  lcd.setCursor(7, LCD_ROWS / 2);
  lcd.print(": ");
  lcd.print(controller2);
  lcd.print("  ");
}

// This function sends a Midi CC.
void sendCC(byte c_num, byte c_val){
  sendMidi(CC,c_num,c_val);
}

void sendMidi(int type, byte partOne, byte partTwo){
#if DEBUG
  Serial.print("MIDI [");
  Serial.print(type);
  Serial.print(", ");
  Serial.print(partOne);
  Serial.print(", ");
  Serial.print(partTwo);  
  Serial.println("]");
#else
  Serial.write(genctrl(type));
  Serial.write(partOne);
  Serial.write(partTwo);
#endif
}

/*! Internal method, don't care about this one.. \n It generates a status byte over a channel and a type, by bitshifting. */
byte genctrl(byte type) {
	byte result = 128;
	result += ((type & 0x07)<<4) & 0x70;
	result += ((midi_channel) & 0x0F);
        //return 0x9f;
	return result;
}

int lastLeft = 0;
int lastRight = 0;

void doMusic() {
  if(modeChanged == true){
    modeChanged = false;
    lcd.clear();
    if(optionMode == CONTROLLER){
      lcd.setCursor(5, 0);
      lcd.print("Controller");
    }else{
      lcd.setCursor(8, 0);
      lcd.print("Notes");
    }
    delay(500);
    lcd.clear();
  }
  
//Read Sensors
#ifdef SENSOR_IR
  int left  = getIrMeasurement(0);//ain 0 = pin 14
  int right = getIrMeasurement(1);//ain 1 = pin 15
#endif
#ifdef SENSOR_HC
  delay(30);
  int left  = getMeasurement(ultrasoundL);
  delay(30);
  int right = getMeasurement(ultrasoundR);
#endif

  if (left < 1) left = lastLeft;
  if (right < 1) right = lastRight;
  
  lastLeft = left;
  lastRight = right;
  
  if(inv_left){
     left = SENSOR_MAX - left;
  }
  if(inv_right){
     right = SENSOR_MAX - right;
  }

#if LCD_ROWS == 4
  lcd.setCursor(0, 1);
  drawBar(left);
  lcd.setCursor(0, 3);
  drawBar(right);
#endif

  //Switch depending on mode
  switch(optionMode){
    case CONTROLLER:
      sendControllers(left, right);
      break;
    case NOTES:
      sendNote(left, right);
      break;
  }
}

void drawBar(int value){
  int length = ((float)value) * (20.0 / SENSOR_RANGE);
  for(int i = 0; i < 20; i++){
    if(i < length){
      lcd.write(0xff);
    }else{
      lcd.write(' ');
    }
  }
}

void doMenu() {
  if (menu_area == MAIN_MENUS) {
    if (last_item != item) {
      last_item = item;

      lcd.clear();

      int total_items = totalItems();

      int top_item = item - 1;
      if (top_item > (total_items - LCD_ROWS)) {
        top_item = total_items - LCD_ROWS;
      }
      if (top_item < 0) {
        top_item = 0;
      }

      for (int line = 0; line < LCD_ROWS; line++) {
        if (line + top_item < total_items) {
            lcd.setCursor(0, line);
          if (item == (line + top_item)) {
            lcd.write(0x7e);
          } else {
            lcd.print(" ");
          }
          strcpy_P(buffer, menu[line+top_item]);
          lcd.print(buffer);
        }
      }
    }
  } else {
    // We're in one of the number choosing menus
    // Work out the current value
    unsigned char cur_value;
    switch (menu_area) {
    case LEFT_CC_NO:
      cur_value = left_cc_number;
      break;
    case RIGHT_CC_NO:
      cur_value = right_cc_number;
      break;
    case MIDI_CHANNEL:
      cur_value = midi_channel + 1;
      break;
    }
    
    // If we need to do anything...
    if (last_item != cur_value) {
      // Work out the max, min and display text
      int max_value, min_value;
      switch (menu_area) {
      case LEFT_CC_NO:
        min_value = 0;
        max_value = 127;
        cur_value = left_cc_number;
        strcpy_P(buffer, controller_menu__left_cc_number);
        break;
      case RIGHT_CC_NO:
        min_value = 0;
        max_value = 127;
        cur_value = right_cc_number;
        strcpy_P(buffer, controller_menu__right_cc_number);
        break;
      case MIDI_CHANNEL:
        min_value = 1;
        max_value = 16;
        cur_value = midi_channel + 1;
        strcpy_P(buffer, top_menu__midi_channel);
        break;
      }
      
      if (last_item == -1) {
        lcd.clear();
        lcd.setCursor((20 - strlen(buffer))/2, 0);
        lcd.print(buffer);
      }
      
      itoa(cur_value, buffer, 10);
      lcd.setCursor(6, LCD_ROWS / 2);
      if (cur_value > min_value) {
        lcd.write(0x7f);
      } else {
        lcd.write(0x20);
      }
      for(int i = 0; i < (4 - strlen(buffer)); i++) lcd.print(" ");
      lcd.print(buffer);
      lcd.print(" ");
      if (cur_value < max_value) {
        lcd.write(0x7e);
      } else {
        lcd.write(0x20);
      }
      last_item = cur_value;
    }
  }
}


//////////////////////////////////////////////////////////////////////////////
// Interrupt servies routine for the click function of the rotary encoder.

void click() {
  delayMicroseconds(2000); // Debounce
  if (digitalRead(ENCODER_CLICK)) return;
  
  last_interrupt = millis();
  
  switch (menu_area) {
  case MAIN_MENUS:
    if (menu == top_menu) {
      switch(item) {
      case 0: // Note Settings
        menu = note_menu;
        item = 0;
        break;
      case 1: // Controller Settings
        menu = controller_menu;
        item = 0;
        break;
      case 2: // MIDI Channel
        menu_area = MIDI_CHANNEL;
        break;
      case 3: // Invert Left Range
        inv_left = !inv_left;
        EEPROM.write(7, inv_left);
        top_menu[3] = 
          inv_left ? 
            controller_menu__inv_left_on : 
            controller_menu__inv_left_off;
        break;
       case 4: // Invert Right Range
        inv_right = !inv_right;
        EEPROM.write(8, inv_right);
        top_menu[4] = 
          inv_right ? 
            controller_menu__inv_right_on : 
            controller_menu__inv_right_off;
        break;
      }
    } else if (menu == note_menu) {
      switch(item) {
      case 0: // Scale
        menu = scale_menu;
        item = scale;
        break;
      case 1: // Root
        menu = root_menu;
        item = root;
        break;
      case 2: // Octave
        menu = octave_menu;
        item = octave + 2;
        break;
      case 3: // Range
        menu = range_menu;
        item = range - 1;
        break;
      case 4: // Back
        menu = top_menu;
        item = 0;
        break;
      }
    } else if (menu == controller_menu) {
      switch(item) {
      case 0: // Left CC Number
        menu_area = LEFT_CC_NO;
        break;
      case 1: // Right CC Number
        menu_area = RIGHT_CC_NO;
        break;
      case 2: // Back
        menu = top_menu;
        item = 1;
        break;
      }
    } else if (menu == scale_menu) {
      menu = note_menu;
      scale = item;
      EEPROM.write(0,scale);
      item = 0;
    } else if (menu == root_menu) {
      menu = note_menu;
      root = item;
      EEPROM.write(1,root);
      item = 1;
    } else if (menu == octave_menu) {
      menu = note_menu;
      octave = item - 2;
      EEPROM.write(2,octave);
      item = 2;
    } else if (menu == range_menu) {
      menu = note_menu;
      range = item + 1;
      EEPROM.write(3,range);
      item = 3;
    }
    makeScale();
    break;
  case LEFT_CC_NO:
    menu_area = MAIN_MENUS;
    menu = controller_menu;
    item = 0;
    break;
  case RIGHT_CC_NO:
    menu_area = MAIN_MENUS;
    menu = controller_menu;
    item = 2;
    break;
  case MIDI_CHANNEL:
    menu_area = MAIN_MENUS;
    menu = top_menu;
    item = 2;
    break;
  }
  
  last_item = -1;
}

//////////////////////////////////////////////////////////////////////////////
// Interrupt service routine for the turn function of the rotary encoder

void turn() {
  // Ignore calls too close together to try and debounce
  if((millis() - last_interrupt) < 5){
    last_interrupt = millis();
    return;
  }
  
  boolean up = digitalRead(ENCODER_B);
  
  //If we're not in a menu, just change mode
  if (last_loop == MUSIC) {
    if (up == true && optionMode == CONTROLLER) {
      optionMode = NOTES;
      EEPROM.write(9, 0);
      modeChanged = true;
    }
    if (up == false && optionMode == NOTES) {
      optionMode = CONTROLLER;
      EEPROM.write(9, 1);
      modeChanged = true;
    }

    return;
  }
  
  last_interrupt = millis();
  
  switch (menu_area) {
  case MAIN_MENUS:
    if (up) {
      int total_items = totalItems();
      item++;
      if (item >= total_items){
        item = total_items - 1;
        blinkLeds();
      }
    } else {
      item--;
      if (item < 0){ 
        item = 0;
        blinkLeds();
      }
    }
    break;
  case LEFT_CC_NO:
    if (up) {
      left_cc_number++;
      // If we're going UP and we end up negative, we've overflown to -128. Set to 127
      if (left_cc_number < 0){
        left_cc_number = 127;
        blinkLeds();
      }
    } else {
      left_cc_number--;
      if (left_cc_number < 0){ 
        left_cc_number = 0;
        blinkLeds();
      }
    }
    EEPROM.write(4, left_cc_number);
    break;
  case RIGHT_CC_NO:
    if (up) {
      right_cc_number++;
      // If we're going UP and we end up negative, we've overflown to -128. Set to 127
      if (right_cc_number < 0){
        right_cc_number = 127;
        blinkLeds();
      }
    } else {
      right_cc_number--;
      if (right_cc_number < 0){
        right_cc_number = 0;
        blinkLeds();
      }
    }
    EEPROM.write(5, right_cc_number);
    break;
  case MIDI_CHANNEL:
    if (up) {
      midi_channel++;
      if (midi_channel > 15){
        midi_channel = 15;
        blinkLeds();
      }
    } else {
      midi_channel--;
      if (midi_channel < 0){
        midi_channel = 0;
        blinkLeds();
      }
    }
    EEPROM.write(6, midi_channel);
    break;
  }
}



void blinkLeds(){
  digitalWrite(NOTE_LED, HIGH);
  digitalWrite(CONTROLLER_LED, HIGH);
  delayMicroseconds(100000);//0.1 sec
  digitalWrite(NOTE_LED, LOW);
  digitalWrite(CONTROLLER_LED, LOW);  
}
