void printBaseInfo() {
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.home();
  lcd.print("CHANNEL: ");
  lcd.print(MIDI_CH);
  lcd.setCursor(0, 1); //col - row
  lcd.print("LAST PC: ");
  lcd.print(0);
  lcd.print("   BNK:");
  lcd.print(0);  
  lcd.setCursor(0, 2);
  lcd.print("LAST CC: ");
  lcd.print(0);
  lcd.print("   VAL:");
  lcd.print(0);
}

void setChannelInfo(int chNum) {
  lcd.setCursor(9, 0);
  lcd.print("   ");
  lcd.setCursor(9, 0);
  lcd.print(chNum);
}

void setLastPCInfo(int pcNum) {
  lcd.setCursor(9, 1);
  lcd.print("   ");
  lcd.setCursor(9, 1);
  lcd.print(pcNum);
}

void setBankInfo(int bankNum) {
  lcd.setCursor(17, 1);
  lcd.print("   ");
  lcd.setCursor(17, 1);
  lcd.print(bankNum);
}

void setLastCCInfo(int ccNum) {
  lcd.setCursor(9, 2);
  lcd.print("   ");
  lcd.setCursor(9, 2);
  lcd.print(ccNum);
}

void setValueInfo(int ccVal) {
  lcd.setCursor(17, 2);
  lcd.print("   ");
  lcd.setCursor(17, 2);
  lcd.print(ccVal);
}