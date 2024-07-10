////////////////////////////////////
//BUTTONS MIDI INFO:
////////////////////////////////////

//"N_BUTTONS" defined in file dmy_midi_footboard_32U4.ino file

//* Put here the type of message you want to send, in the same order you declared the button pins
// "NN" for Note Number | "CC" for Control Change | "T" for Note Number but in toggle mode | "PC" for Program Change
byte MESSAGE_TYPE[N_BUTTONS] = { CC, CC, CC, CC, CC, CC, CC, CC, CC, CC, CC, CC, PC, PC, PC, PC};

//* Put here the number of the message you want to send, in the right order, no matter if it's a note number, CC (or MACKIE), Program Change
byte MESSAGE_NUM[N_BUTTONS] =  { 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20,  0,  1,  2,  3};


////////////////////////////////////
//POTENTIOMETES MIDI INFO:
////////////////////////////////////
//"totalPots" defined in file dmy_midi_footboard_32U4.ino file
//Config in the following order: All ports connected direct to the arduino followed by the pots connected in the mux, from C0 to Cn
//NA disables the potentiometer in the respective mux channel. 
//byte MESSAGE_TYPE_MUX_POT[MUX_POT_COUNT] = { NA, NA, CC, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA };
#ifdef TEST_EACH_POT_CH
byte MESSAGE_TYPE_MUX_POT[MUX_POT_COUNT] = { CC, CC, CC, CC, CC, CC, CC, CC, CC, CC, CC, CC, CC, CC, CC, CC };
#else
//byte MESSAGE_TYPE_MUX_POT[MUX_POT_COUNT] = { CC, NA, NA, CC, NA, NA, CC, CC, CC, CC, CC, CC, CC, CC, NA, NA };
byte MESSAGE_TYPE_MUX_POT[MUX_POT_COUNT] = { CC, NA, NA, CC, CC, CC, CC, CC, CC, CC, CC, CC, CC, CC, CC, CC };
#endif

byte MUX_POT_CC[MUX_POT_COUNT]           = { 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95 };

byte MESSAGE_TYPE_ARDUINO_POT[ARDUINO_POT_COUNT] = {  };
byte ARDUINO_POT_CC[ARDUINO_POT_COUNT]           = {  };