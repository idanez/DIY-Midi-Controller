void setup() {

#ifdef DEBUG
//  int baudRate = 115200;
  int baudRate = 9600;
  Serial.begin(baudRate);
  Serial.println("Debug mode");
  Serial.println();
#endif

#ifdef DEBUG
  while (!Serial) {
    Serial.println("waiting...");
  }
  Serial.println();
#endif

  delay(100);
  btnMux.begin(); // Initialize the button Multiplexer
  potMux.begin(); // Initialize the pot Multiplexer

  printBaseInfo();

  for (int i = 0; i < TOTAL_POT_COUNT; i++) {
    responsivePot[i] = ResponsiveAnalogRead(0, true, snapMultiplier);
    responsivePot[i].setAnalogResolution(1024);
  }

  threadPotentiometers.setInterval(5);         // every how many millisiconds
  threadPotentiometers.onRun(potentiometers);  // the function that will be added to the thread
  cpu.add(&threadPotentiometers);              // add every thread here
  
  //threadBanksWithButtons.setInterval(30);
  //threadBanksWithButtons.onRun(banksWithButtons);
  //cpu.add(&threadBanksWithButtons);

}
