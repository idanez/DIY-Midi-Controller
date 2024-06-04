#ifdef USING_POTENTIOMETERS

/////////////////////////////////////////////
// variables you don't need to change
int reading = 0;

// Responsive Analog Read
float snapMultiplier = 0.01;                      // (0.0 - 1.0) - Increase for faster, but less smooth reading
ResponsiveAnalogRead responsivePot[N_POTS] = {};  // creates an array for the responsive pots. It gets filled in the Setup.

int potCState[N_POTS] = { 0 };  // Current state of the pot
int potPState[N_POTS] = { 0 };  // Previous state of the pot
int potVar = 0;                 // Difference between the current and previous state of the pot

int potMidiCState[N_POTS] = { 0 };  // Current state of the midi value
int potMidiPState[N_POTS] = { 0 };  // Previous state of the midi value

//#ifdef USING_MACKIE
int PBVal[N_POTS] = { 0 };
//#endif

boolean potMoving = true;             // If the potentiometer is moving
unsigned long PTime[N_POTS] = { 0 };  // Previously stored time
unsigned long timer[N_POTS] = { 0 };  // Stores the time that has elapsed since the timer was reset

// one pole filter
// y[n] = A0 * x[n] + B1 * y[n-1];
// A = 0.15 and B = 0.85
//float filterA = 0.3;
//float filterB = 0.7;


/////////////////////////////////////////////
// Function
void potentiometers() {

  // reads the pins from arduino
  for (int i = 0; i < N_POTS_ARDUINO; i++) {
    reading = analogRead(POT_ARDUINO_PIN[i]);
    responsivePot[i].update(reading);
    potCState[i] = responsivePot[i].getValue();
    //potCState[i] = reading;
  }

#ifdef USING_MUX
  //It will happen if using a mux
  int nPotsPerMuxSum = N_POTS_ARDUINO;  //offsets the buttonCState at every mux reading

  // reads the pins from every mux
  for (int j = 0; j < N_MUX; j++) {
    for (int i = 0; i < N_POTS_PER_MUX[j]; i++) {
      //potCState[i + nPotsPerMuxSum] = mux[j].readChannel(POT_MUX_PIN[j][i]);
      reading = mux[j].readChannel(POT_MUX_PIN[j][i]);
      responsivePot[i + nPotsPerMuxSum].update(reading);
      potCState[i + nPotsPerMuxSum] = responsivePot[i + nPotsPerMuxSum].getValue();
    }
    nPotsPerMuxSum += N_POTS_PER_MUX[j];
  }
#endif

  for (int i = 0; i < N_POTS; i++) {  // Loops through all the potentiometers

    potCState[i] = clipValue(potCState[i], potMin, potMax);
    //potMidiCState[i] = map(potCState[i], potMin, potMax, 0, 127); // Maps the reading of the potCState to a value usable in midi
    potMidiCState[i] = map(potCState[i], potMin, potMax, 0, 127);  // Maps the reading of the potCState to a value usable in midi
    potMidiCState[i] = clipValue(potMidiCState[i], 0, 127);

    //potMidiCState[i] = potCState[i] >> 3; // Maps the reading of the potCState to a value usable in midi

    PBVal[i] = map(potCState[i], potMin, potMax, 0, 16383);
    potVar = abs(potCState[i] - potPState[i]);  // Calculates the absolute value between the difference between the current and previous state of the pot

    if (potVar > varThreshold) {  // Opens the gate if the potentiometer variation is greater than the threshold
      PTime[i] = millis();        // Stores the previous time
    }

    timer[i] = millis() - PTime[i];  // Resets the timer 11000 - 11000 = 0ms

    if (timer[i] < TIMEOUT) {  // If the timer is less than the maximum allowed time it means that the potentiometer is still moving
      potMoving = true;
    } else {
      potMoving = false;
    }

    if (potMoving == true) {  // If the potentiometer is still moving, send the change control
      if (potMidiPState[i] != potMidiCState[i]) {
#ifdef DEBUG
        printDebugMsgs();
#endif
        potPState[i] = potCState[i];  // Stores the current reading of the potentiometer to compare with the next
        potMidiPState[i] = potMidiCState[i];
      }
    }
  }
}




#ifdef DEBUG
void printDebugMsgs() {

      Serial.print("Pot: ");
      Serial.print(i);
      Serial.print("  |  ch: ");
      Serial.print(POT_MIDI_CH);

      switch (MESSAGE_TYPE_POT[i]) {
        case 1:  // CC
          Serial.print("  |  cc: ");
          break;
        case 4:  // PB
          Serial.print("  |  pb: ");
          break;
      }

#ifdef USING_CUSTOM_CC_N
      Serial.print(POT_CC_N[i]);
#else
      Serial.print(CC_NUMBER + i);
#endif

      switch (MESSAGE_TYPE_POT[i]) {
        case 1:  // CC
          Serial.print("  |  value: ");
          Serial.print(potMidiCState[i]);
          Serial.print("  |  Responsive Value: ");
          Serial.print(responsivePot[i].getValue());
          break;
        case 4:  // PB
          Serial.print("  |  Pitch Bend Val: ");
          Serial.print(PBVal[i]);
          Serial.print("  |  Responsive Value: ");
          Serial.print(responsivePot[i].getValue());
          break;
      }
      Serial.println();
}
#endif


#endif  // USING_POTENTIOMETERS
