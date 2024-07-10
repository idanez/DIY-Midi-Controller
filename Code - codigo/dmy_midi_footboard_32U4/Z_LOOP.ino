void loop() {
  //Teste de sleep para ver se melhora a reprogramacao
  delay(1);
  // it will read MIDI incoming messages if using ATMEGA32U4
  MIDIread();

  buttonsLoop();

  potentiometers();

  //cpu.run(); // for threads

}
