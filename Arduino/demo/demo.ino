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

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

bool b = false;

void setup() {
  Serial.begin(115200);

  pinMode(D5, OUTPUT);
  digitalWrite(D5, b);

  Wire.begin(D2, D1);
  lcd.begin(16,2);
  lcd.home();
  lcd.print("Hello world");
}

// the loop function runs over and over again forever
void loop() {
  if (Serial.available()>0)
  {
    // Blink LED to indicate we've received serial data
    b = !b;
    digitalWrite(D5, b);
    
    String s = Serial.readStringUntil('\n');   // Until CR (Carriage Return)
    //s.replace("#", "");
       
    Serial.print("Received ");
    Serial.println(s);
    Serial.println(millis() / 1000);

    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
    // print the number of seconds since reset:
    lcd.print(millis() / 1000);
  }
}
