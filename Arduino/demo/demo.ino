/*
 * Example schematic
 * https://michaelthessel.com/15-wireless-display/
 * 
 * LiquidCrystal library docs
 * https://www.arduino.cc/en/Tutorial/HelloWorld
 * 
 * LCD - NodeMCU
 * D4 - D0
 * D5 - D1
 * D6 - D2
 * D7 - D3
 * 
 * RS - D4
 * E  - D
 */

#include <LiquidCrystal.h>

LiquidCrystal lcd(D4, D5, D0, D1, D2, D3);

void setup() {
  // initialize GPIO 2 as an output.
  Serial.begin(115200);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");
}

// the loop function runs over and over again forever
void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis() / 1000);
  Serial.println(millis() / 1000);
  //Serial.println("Hello world");
  delay(1000);
}
