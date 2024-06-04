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

  btnMux.begin(); // Initialize the Multiplexer

  printBaseInfo();
  
  //threadBanksWithButtons.setInterval(30);
  //threadBanksWithButtons.onRun(banksWithButtons);
  //cpu.add(&threadBanksWithButtons);

}
