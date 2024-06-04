
// debounce
unsigned long lastDebounceTime[N_BUTTONS] = { 0 };  // the last time the output pin was toggled

/////////////////////////////////////////////
// Function
void buttonsLoop() {

  //SCANS THE MUX TO SET THE VALUE HIGH OR LOW TO EACH BUTTON
  for (int i = 0; i < N_BUTTONS; i++) { // Reads buttons on Multiplexer
    int buttonReading = btnMux.readChannel(muxButtonPin[i]);
    if (buttonReading > buttonMuxThreshold) {
      buttonCState[i] = HIGH;
    }
    else {
      buttonCState[i] = LOW;
    }
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  for (int i = 0; i < N_BUTTONS; i++) {  // Read the buttons connected to the Arduino
    bool sendMsg = true;

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {

      if (buttonPState[i] != buttonCState[i]) {
        lastDebounceTime[i] = millis();

        if (buttonCState[i] == HIGH) {  // if button is pressed
          sendMsg = true;
          if(MESSAGE_TYPE[i] == CC) {
            if(lastCCMsgSent == MESSAGE_NUM[i]){
              CC_MSG_VALUE[i] = CC_MSG_VALUE[i] == 127 ? 0 : 127;
            } else {
              CC_MSG_VALUE[i] = 127;
            }
          }
        } else {
          sendMsg = SEND_MSG_ON_BTN_RELEASE;
          //if(MESSAGE_TYPE[i] == CC)
//            CC_MSG_VALUE[i] = 0;
        }

        if(sendMsg) 
        {
          if(i == 0 || i == 1) {
            processMidiChannelChange(i);               
          } if(i == 2 || i == 3) {
            processPCBank(i);
          } else {
            switch (MESSAGE_TYPE[i]) {
              case 0:
                processNNMsg(i);
                break;
              case 1:
                processCCMsg(i);
                break;
              case 2:
                processToggleMsg(i);
              break;
              case 3:
                processPCMsg(i);
                break;
            }
          }
#ifdef DEBUG
          printDbgBtnMessage(i);            
#endif
        }

        buttonPState[i] = buttonCState[i];
      }
    }
  }
}


void processNNMsg(int& btnIndex)
{
  // noteOn(MIDI_CH, MESSAGE_NUM[i] + octave, velocity[i]);  // channel, note, velocity
  // MidiUSB.flush();
}


void processCCMsg(int& btnIndex)
{
  controlChange(MIDI_CH, MESSAGE_NUM[btnIndex], CC_MSG_VALUE[btnIndex]);  //  (channel, CC number,  CC value)
  MidiUSB.flush();
  lastCCMsgSent = MESSAGE_NUM[btnIndex];
  setLastCCInfo(MESSAGE_NUM[btnIndex]);
  setValueInfo(CC_MSG_VALUE[btnIndex] == 127 ? 0 : 127); //Inverte no display pois o Amplitube esta cosniderando como valor 0 = ligado e 127 = desligado
}

void processToggleMsg(int& btnIndex)
{
  // if (buttonCState[i] == LOW) {
  //   toggleVelocity[i] = !toggleVelocity[i] * 127;  // changes the velocity between 0 and 127 each time one press a button
  //   noteOn(MIDI_CH, MESSAGE_NUM[i] + octave, toggleVelocity[i]);      // channel, note, velocity
  //   MidiUSB.flush();
  // }
}


void processPCMsg(int& btnIndex)
{
  int pcNum = MESSAGE_NUM[btnIndex] + (4*currentBank);
  programChange(MIDI_CH, pcNum);
  MidiUSB.flush();
  setLastPCInfo(pcNum + 1); //Mostra como indice 1 pois o Amplitube recebe o patch como indice 0, mas mostra como indice 1
}

void processPCBank(int btnIndex) {
  btnIndex == 3 ? currentBank++ : currentBank--;
   if(currentBank > 127) 
    currentBank = 0;
  if(currentBank < 0)
    currentBank = 127;
  setBankInfo(currentBank);
}

void processMidiChannelChange(int btnIndex) {
  btnIndex == 1 ? MIDI_CH++ : MIDI_CH--;
  if(MIDI_CH > 127) 
    MIDI_CH = 1;
  if(MIDI_CH < 1)
    MIDI_CH = 127;
  setChannelInfo(MIDI_CH);        
}

#ifdef DEBUG
void printDbgBtnMessage(int i) {
  Serial.print("Button: ");
  Serial.print(i);
  Serial.print("  | ch: ");
  Serial.print(MIDI_CH);
  Serial.print("  | ");
  char * name = "NN";
  switch (MESSAGE_TYPE[i]){
    case 1:
      name = "CC";
      break;
    case 2:
      name = "T";
      break;
    case 3:
      name = "PC";
    case 4:
      name = "PB";
    break;          
  }
  Serial.print(name);
  Serial.print(" : ");
  Serial.print(MESSAGE_NUM[i]);
  Serial.print(" | CC MSG VALUE: ");
  Serial.print(CC_MSG_VALUE[i]);
}
#endif
