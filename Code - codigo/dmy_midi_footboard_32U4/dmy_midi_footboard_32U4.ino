/////////////////////////////////////////////
// LIBRARIES
// -- Defines the MIDI library -- //

// if using with ATmega32U4 - Micro, Pro Micro, Leonardo...
#include <MIDIUSB.h>

//////////////////////
// Using a cd4067 multiplexer
#include <Multiplexer4067.h>  // Multiplexer CD4067 library >> https://github.com/sumotoy/Multiplexer4067

//////////////////////
// Threads
//#include <Thread.h>            // Threads library >> https://github.com/ivanseidel/ArduinoThread
//#include <ThreadController.h>  // Same as above

// include the ResponsiveAnalogRead library
#include <ResponsiveAnalogRead.h>  // https://github.com/dxinteractive/ResponsiveAnalogRead

//////////////////////
// DISPLAY
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//#define DEBUG 1

///////////////////////////////////////////
// MULTIPLEXERS
//* Define s0, s1, s2, s3, and x pins
#define btnMuxS0 9
#define btnMuxS1 8
#define btnMuxS2 7
#define btnMuxS3 6
#define btnMuxSig 10

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Initializes the multiplexer
// MILTIPLEXER
//const uint8_t S0,const uint8_t S1,const uint8_t S2,const uint8_t S3,const uint8_t SIG)
Multiplexer4067 btnMux = Multiplexer4067(btnMuxS0, btnMuxS1, btnMuxS2, btnMuxS3, btnMuxSig);

const bool SEND_MSG_ON_BTN_RELEASE = false;  //* Set this value to true if the button must send a message when it is released

/////////////////////////////////////////////
// MIDI CHANNEL
byte MIDI_CH = 1;

// Types of MIDI messages to send - DO NOT TOUCH
//byte NN = 0;  // Note number or MACKIE
byte CC = 1;  // Control change
//byte T = 2;   // Toggle
byte PC = 3;  // Program Change
//byte PB = 4;  // Pitch Bend

/////////////////////////////////////////////
// BUTTONS
const byte N_BUTTONS = 16; // How many digital inputs used for the buttons on the Multiplexer

//Pinos do MUX onde os botoes estao ligados
const int muxButtonPin[N_BUTTONS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; // Array created for order of pins corresponding to digital ports being used
int buttonCState[N_BUTTONS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // Current state of the digital port
int buttonPState[N_BUTTONS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // Previous state of the digital port.

int currentBank = 0;
int buttonMuxThreshold = 1000; //850
// DEBOUNCE
unsigned long debounceDelay = 120;  //* the debounce time; increase if the output flickers

//* Put here the type of message you want to send, in the same order you declared the button pins
// "NN" for Note Number | "CC" for Control Change | "T" for Note Number but in toggle mode | "PC" for Program Change
byte MESSAGE_TYPE[N_BUTTONS] = { CC, CC, CC, CC, CC, CC, CC, CC, CC, CC, CC, CC, PC, PC, PC, PC};

//* Put here the number of the message you want to send, in the right order, no matter if it's a note number, CC (or MACKIE), Program Change
byte MESSAGE_NUM[N_BUTTONS] =  { 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20,  0,  1,  2,  3};
byte CC_MSG_VALUE[N_BUTTONS]  {0};
byte lastCCMsgSent = 0;
