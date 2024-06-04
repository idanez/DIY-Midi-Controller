// - - - - - - - - - - - - - - - - - - - - -
// FUNCTIONS TO SEND MIDI OUT OF THE USB

// Arduino (pro)micro midi functions MIDIUSB Library
// Functions to SEND MIDI
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = { 0x09, 0x90 | channel, pitch, velocity };
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = { 0x08, 0x80 | channel, pitch, velocity };
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = { 0x0B, 0xB0 | channel, control, value };
  MidiUSB.sendMIDI(event);
}

void pitchBend(byte channel, int value) {
  byte lowValue = value & 0x7F;
  byte highValue = value >> 7;
  midiEventPacket_t pitchBend = { 0x0E, 0xE0 | channel, lowValue, highValue };
  MidiUSB.sendMIDI(pitchBend);
}

/*
  Program Change message: This message is used to change the instrument or program on a MIDI device.
  The status byte for this message is 0xC0, and the data byte contains the new program number (0-127).
*/
void programChange(byte channel, byte program) {
  midiEventPacket_t message = { 0x0C, 0xC0 | channel, program, 0 };
  MidiUSB.sendMIDI(message);
}
/*
  Channel Pressure message: This message is used to send aftertouch or pressure data from a MIDI device.
  The status byte for this message is 0xD0, and the data byte contains the pressure value (0-127).
*/
void channelPressure(byte channel, byte pressure) {
  midiEventPacket_t message = { 0x0D, 0xD0 | channel, pressure, 0 };
  MidiUSB.sendMIDI(message);
}

/*
  Aftertouch message: This message is used to send pressure data for a single note on a MIDI device.
  The status byte for this message is 0xA0, and the data bytes contain the note number (0-127) and the pressure value (0-127).
*/
void aftertouch(byte channel, byte note, byte pressure) {
  midiEventPacket_t message = { 0x02, 0xA0 | channel, note, pressure };
  MidiUSB.sendMIDI(message);
}

/*
  Polyphonic Aftertouch message: This message is used to send pressure data for multiple notes on a MIDI device.
  The status byte for this message is 0xD0, and the data bytes contain the note number (0-127) and the pressure value (0-127).
*/
void polyAftertouch(byte channel, byte note, byte pressure) {
  midiEventPacket_t message = { 0x02, 0xD0 | channel, note, pressure };
  MidiUSB.sendMIDI(message);
}

/*
  System Common messages: These are messages used for system-wide control of MIDI devices.
  There are several types of System Common messages, including Song Position Pointer (0xF2), Song Select (0xF3),
  Tune Request (0xF6), and System Exclusive (0xF0).
  The data bytes for these messages vary depending on the type of message being sent.
  THESE ONES ARE NOT TESTED YET
*/
void songPositionPointer(byte positionLSB, byte positionMSB) {
  midiEventPacket_t message = { 0x02, 0xF2, positionLSB, positionMSB };
  MidiUSB.sendMIDI(message);
}

void songSelect(byte songNumber) {
  midiEventPacket_t message = { 0x01, 0xF3, songNumber, 0 };
  MidiUSB.sendMIDI(message);
}

void tuneRequest() {
  midiEventPacket_t message = { 0x01, 0xF6, 0, 0 };
  MidiUSB.sendMIDI(message);
}

void systemExclusive(byte* data, byte length) {
  midiEventPacket_t message = { (length > 3 ? 0x04 : length - 1), 0xF0, data[0], data[1] };
  for (int i = 2; i < length; i++) {
    message.byte2 = data[i];
    MidiUSB.sendMIDI(message);
  }
  message.byte2 = 0xF7;
  MidiUSB.sendMIDI(message);
}

// - - - - - - - - - - - - - - - - - - - - -
// Functions to RECEIVE MIDI (MIDIUSB lib)
void MIDIread() {

  midiEventPacket_t rx = MidiUSB.read();

  switch (rx.header) {
    case 0:
      break;  //No pending events

    case 0x9:
      handleNoteOn(
        rx.byte1 & 0xF,  //channel
        rx.byte2,        //pitch
        rx.byte3         //velocity
      );
      break;

    case 0x8:
      handleNoteOff(
        rx.byte1 & 0xF,  //channel
        rx.byte2,        //pitch
        rx.byte3         //velocity
      );
      break;

    case 0xB:
      handleControlChange(
        rx.byte1 & 0xF,  //channel
        rx.byte2,        //cc
        rx.byte3         //value
      );
      break;

    case 0xE:
      //      handlePitchBend(
      //        rx.byte1 & 0xF,  //channel
      //        rx.byte2,        //LSB
      //        rx.byte3         //MSB
      //      );
      break;

    case 0xC:
      handleProgramChange(
        rx.byte1 & 0xF,  //channel
        rx.byte2         //program number
      );
      break;

    case 0xD:
      handleAftertouch(
        rx.byte1 & 0xF,  //channel
        rx.byte2         //pressure
      );
      break;

      // case 0xF:
      //   {
      //     // The first byte of a system exclusive message is always 0xF0
      //     if (rx.byte1 == 0xF0) {
      //       // We have a valid system exclusive message
      //       byte* data = rx.data;
      //       unsigned int length = rx.byte2;
      //       handleSysEx(data, length);
      //     }
      //   }
      //   break;
  }

  //  if (rx.header != 0) {
  //    Serial.print("Unhandled MIDI message: ");
  //    Serial.print(rx.byte1 & 0xF, DEC);
  //    Serial.print("-");
  //    Serial.print(rx.byte1, DEC);
  //    Serial.print("-");
  //    Serial.print(rx.byte2, DEC);
  //    Serial.print("-");
  //    Serial.println(rx.byte3, DEC);
  //  }
}

// - - - - - - - - - - - - - - - - - - - - -
// FUNCTIONS TO DO SOMETHING WITH THE MIDI INCOMING MESSAGES

void handleNoteOn(byte channel, byte number, byte value) {

}

// - - - - - - - - - - - - - - - - - - - - -

void handleNoteOff(byte channel, byte number, byte value) {

}

// - - - - - - - - - - - - - - - - - - - - -

void handleControlChange(byte channel, byte number, byte value) {
}

void handleProgramChange(byte channel, byte program) {
  // Do something with the program change message
}

void handleAftertouch(byte channel, byte pressure) {
  // Do something with the aftertouch message
}

void handleSysEx(byte* data, unsigned int length) {
  // Do something with the system exclusive message
}

////////////////////////////////////////////
