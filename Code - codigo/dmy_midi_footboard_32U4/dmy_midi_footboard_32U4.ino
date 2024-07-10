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
#include <Thread.h>            // Threads library >> https://github.com/ivanseidel/ArduinoThread
#include <ThreadController.h>  // Same as above

// include the ResponsiveAnalogRead library
#include <ResponsiveAnalogRead.h>  // https://github.com/dxinteractive/ResponsiveAnalogRead

//////////////////////
// DISPLAY
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


//############################################## DEBUG
//#define DEBUG 1
//#define TEST_EACH_POT_CH 1
#ifdef TEST_EACH_POT_CH
#define POT_TEST_TIMEOUT 10000
#endif


#define ATMEGA32U4 1
#define USING_POTENTIOMETERS 1
#define USING_CUSTOM_CC_N 1

///////////////////////////////////////////
// MULTIPLEXERS
//* Define s0, s1, s2, s3, and x pins
#define btnMuxS0 8
#define btnMuxS1 7
#define btnMuxS2 6
#define btnMuxS3 5
#define btnMuxSig 9

#define potMuxS0 10
#define potMuxS1 16
#define potMuxS2 14
#define potMuxS3 15
#define potMuxSig A0

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Initializes the multiplexer
// MILTIPLEXER
//const uint8_t S0,const uint8_t S1,const uint8_t S2,const uint8_t S3,const uint8_t SIG)
Multiplexer4067 btnMux = Multiplexer4067(btnMuxS0, btnMuxS1, btnMuxS2, btnMuxS3, btnMuxSig);

Multiplexer4067 potMux = Multiplexer4067(potMuxS0, potMuxS1, potMuxS2, potMuxS3, potMuxSig);

const bool SEND_MSG_ON_BTN_RELEASE = false;  //* Set this value to true if the button must send a message when it is released

/////////////////////////////////////////////
// MIDI CHANNEL
byte MIDI_CH = 1;

// Types of MIDI messages to send - DO NOT TOUCH
//byte NN = 0;  // Note number or MACKIE
const byte CC = 1;  // Control change
//byte T = 2;   // Toggle
const byte PC = 3;  // Program Change
const byte PB = 4;  // Pitch Bend

const byte NA = 9; // Disables the input

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

byte CC_MSG_VALUE[N_BUTTONS]  {0};
byte lastBtnCCSent = 0;
byte lastBtnPCSent = 0;


//----------------------------------------------------------
//POTENCIOMETERS
//----------------------------------------------------------
const byte MUX_POT_COUNT = 16;
const byte ARDUINO_POT_COUNT = 0;  //* número de potenciômetros conectados diretamente ao Arduino
const byte TOTAL_POT_COUNT = MUX_POT_COUNT + ARDUINO_POT_COUNT;
const byte POT_ARDUINO_PIN[ARDUINO_POT_COUNT] = { }; //* pinos de cada potenciômetro conectado diretamente ao Arduino A1, A2, etc..

//Besides the threshold, there will be added a "Gate" of time when the readins will happen.
//When the pot stops moving, it will be open for and amount of time, then it "closes", stoping the readings
//The value here ar in milliseconds
const int POT_TIMEOUT = 700;      //* Quantidade de tempo que o potenciômetro será lido após exceder o varThreshold
// Para varThreshold, geralmente uso "8" para o Arduino normal e "30" para o ESP32
const byte potReadThreshold = 50;  //* Limite para a variação do sinal do potenciômetro - Aumente se estiver usando ESP32

bool disableAllPots = false;

#ifdef TEST_EACH_POT_CH
int lastTime = millis();
#endif

// coloque aqui a leitura mínima e máxima no potCState
// coloque um pouco mais em potMin e um pouco menos em potMax
// IMPORTANTE:
// Arduinos regulares têm resolução de 10 bits: 0 - 1023
// As placas ESP32 têm resolução de 12 bits: 0 - 4095
//const int potMin = 20;
//const unsigned int potMax = 940;
