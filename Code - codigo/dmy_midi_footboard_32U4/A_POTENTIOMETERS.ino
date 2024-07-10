
int potReading[TOTAL_POT_COUNT] = {0};

int potVal[TOTAL_POT_COUNT] = {0};
int potPrevVal[TOTAL_POT_COUNT] = {0};
int potMidiVal[TOTAL_POT_COUNT] = {0};
int potPrevMidiVal[TOTAL_POT_COUNT] = {0};

unsigned long potTime[TOTAL_POT_COUNT] = {0};
unsigned long potPrevTime[TOTAL_POT_COUNT] = {0};

int currPot = 0;

//This is to make the reading more or less smooth
// (0.0 - 1.0) - Increase for faster, but less smooth reading
float snapMultiplier = 0.01;
ResponsiveAnalogRead responsivePot[TOTAL_POT_COUNT] = {};  // creates an array for the responsive pots. It gets filled in the Setup.

//int muxChannel = 0;
byte lastPotCCUsed = 0;

byte midiValSamples[5] = { 0, 0, 0, 0, 0 };

/////////////////////////////////////////////
// Function
void potentiometers() {

  processPotsInMux();
  processPotsInArduino();
  
}

void processPotsInMux() {
  if (disableAllPots) {
    return;
  }
  //Restringindo o loop para a quantidade de pots no teste, apesar dos arrays serem iniciados com a quantidade maxima do multiplexer.
  //Fazendo isso, pois caso nao exista potenciometro conectado em alguns dos canais do multiplexer, quando esse for selecionado no 
  //loop será feito leituras erraticas, impactando o teste  
  for (int muxChannel = 0; muxChannel < 16; muxChannel++) {
  //for (muxChannel = 0; muxChannel < MUX_POT_COUNT; muxChannel++) {
    
#ifdef TEST_EACH_POT_CH
    bool usePot = isPotAvailableToTest(muxChannel);
    if(!usePot) {
      continue;
    }
#endif

    if(MESSAGE_TYPE_MUX_POT[muxChannel] != 9) {

      potReading[muxChannel] = potMux.readChannel(muxChannel);
      responsivePot[muxChannel].update(potReading[muxChannel]);

      potVal[muxChannel] = responsivePot[muxChannel].getValue();
      //potVal[muxChannel] = potMux.readChannel(muxChannel);

      int midiVal;
      if(muxChannel < 3) {
        //Reduzo o range dos valores para os pedais conectados via jack P10 pois, em testes eles nunca chegam no 0 nem no 127
        if(potVal[muxChannel] < 130)
          midiVal = 0;
        else
          //midiVal = map(potVal[muxChannel], 130, 930, 0, 127);
          midiVal = map(potVal[muxChannel], 0, 1024, 0, 127);
      } else {
        if(potVal[muxChannel] < 20)
          midiVal = 0;
        else
        //potMidiVal[muxChannel] = map(potVal[muxChannel], 0, 1023, 0, 127);
        //Reduzi um pouco a sensibilidade do pot
          midiVal = map(potVal[muxChannel], 0, 1024, 0, 127);
      }

      if(midiVal < 0)
        midiVal = 0 ;
      if(midiVal > 127)
        midiVal = 127;
      potMidiVal[muxChannel] = midiVal;

      //bool isFluctuating = checkFluctuation(muxChannel, midiVal);
      //if(isFluctuating)
      //  continue;
      
      //a funcao abs basicamente forca qualquer resultado da conta abaixo a ter um valor positivo, 
      // por exemplo, caso a conta seja 10 - 20, o resultado seria -10, o abs transforma em 10
      //int potVar = abs(potVal[muxChannel] - potPrevVal[muxChannel]);
      int potVarDiff = potVal[muxChannel] - potPrevVal[muxChannel];
      if(potVarDiff < 0)
        potVarDiff = potVarDiff * -1;

      if(potVarDiff > potReadThreshold) {
        potPrevTime[muxChannel] = millis();
      }

      potTime[muxChannel] = millis() - potPrevTime[muxChannel];

      if(potTime[muxChannel] < POT_TIMEOUT) {
        if(potMidiVal[muxChannel] != potPrevMidiVal[muxChannel]) {
          switch (MESSAGE_TYPE_MUX_POT[muxChannel]) {
            case CC: // CC
              controlChange(MIDI_CH, MUX_POT_CC[muxChannel], potMidiVal[muxChannel]);  //  (channel, CC number,  CC value)
              MidiUSB.flush();
              if(MUX_POT_CC[muxChannel] != lastPotCCUsed) {
                setLastPotCCInfo(MUX_POT_CC[muxChannel]);
              }
              setPotValueInfo(potMidiVal[muxChannel]);
              lastPotCCUsed = MUX_POT_CC[muxChannel];

  #ifdef DEBUG
              printDbgPotMessages(muxChannel, true);
  #endif

              break;
            case PB:  // PB
              //NOT USING ANY OTHER MESSAGE TYPE FOR NOW.
              //LEFT HERE FOR FUTURE NEEDS
              break;
              case NA:
                //Just ignore it
              break;
          }

          potPrevMidiVal[muxChannel] = potMidiVal[muxChannel];
        }
        potPrevVal[muxChannel] = potVal[muxChannel];
      }
    }
  }

}

//TODO
void processPotsInArduino() {

}


bool checkFluctuation(int muxChannel, int midiVal) {

  bool isSampleComplete = false;
  bool isFluctuating = false;
  //Precisa ter um desse para cada pot
  if(muxChannel == 50) {

    for(int samplesIdx = 0; samplesIdx < 5; samplesIdx++) {
      if(midiValSamples[samplesIdx] == 0) {
        midiValSamples[samplesIdx] = midiVal;
        break;        
      }      
      isSampleComplete = true;
    }

    if(isSampleComplete) {
      bool isGoingUp = false;
      if(midiValSamples[0] > midiValSamples[1]) {
        isGoingUp = true;
      }
      midiValSamples[0] = 0;
      for (int idxSamples = 1 ; idxSamples < 4; idxSamples++) {
        if (isGoingUp) {
          if(midiValSamples[idxSamples] > midiValSamples[idxSamples+1]) {
            isFluctuating = true;
#ifdef DEBUG
            Serial.println("Flutando pra Cima: ");              
#endif
            break;
          }
        } else {
          if(midiValSamples[idxSamples] < midiValSamples[idxSamples+1]) {
#ifdef DEBUG
            Serial.println("Flutando pra BAIXO: ");
#endif
            isFluctuating = true;
            break;
          }
        }
        midiValSamples[idxSamples] = 0;
      }
    }
#ifdef DEBUG
    Serial.print("Idx: ");
    Serial.print(muxChannel);
    Serial.print("Val: ");
    Serial.println(midiVal);
#endif
 
  }
  return isFluctuating;
}

#ifdef TEST_EACH_POT_CH
bool isPotAvailableToTest(int muxChannel) {
  if(currPot == muxChannel) {
    int curTime = millis();
    if(curTime - lastTime >= POT_TEST_TIMEOUT ) {
      lastTime = millis();
      if(currPot == 15)
        currPot = 0;
      else
        currPot += 1;
    }
    return true;
  } else {
    return false;
  }
}
#endif

#ifdef DEBUG
void printDbgPotMessages(int i, bool isMux) {
  Serial.print("Pot: ");
  Serial.print(i);
  Serial.print("  |  ch: ");
  Serial.print(MIDI_CH);

  byte msgType = MESSAGE_TYPE_MUX_POT[i];
  if(!isMux) {
    msgType = MESSAGE_TYPE_ARDUINO_POT[i];
  }
  switch (msgType) {
    case 1:  // CC
      Serial.print("  |  cc: ");
      break;
    case 4:  // PB
      Serial.print("  |  pb: ");
      break;
  }

  if(isMux)
    Serial.print(MUX_POT_CC[i]);
  else
    Serial.print(ARDUINO_POT_CC[i]);


  switch (msgType) {
    case 1:  // CC
      Serial.print("  |  value: ");
      Serial.print(potMidiVal[i]);
      Serial.print("  |  Responsive Value: ");
      Serial.print(responsivePot[i].getValue());
      break;   
  }

  Serial.print("  |  PrevTime: ");
  Serial.print(potPrevTime[i]);
    Serial.print("  |  CurTime: ");
  Serial.print(potTime[i]);

  Serial.println();
}
#endif