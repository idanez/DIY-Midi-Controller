void loop() {

  // it will read MIDI incoming messages if using ATMEGA32U4
  MIDIread();

  buttonsLoop();

  //cpu.run(); // for threads

}
