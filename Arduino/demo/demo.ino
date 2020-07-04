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

#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
StaticJsonDocument<200> doc;
bool b = false;

void setup() {
  Serial.begin(9600, SERIAL_8N1);
  //Serial1.begin(9600, SERIAL_8N1);
  Serial.setTimeout(200);

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
    // {"pitch":..., "roll":...}
    // eg {"pitch":10.1, "roll":-1.7}
    String s = Serial.readStringUntil('\n');   // Until CR (Carriage Return)
    Serial.println("Received ");
    Serial.println(s);

    DeserializationError err = deserializeJson(doc, s);
    if (err) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(err.c_str());
    }

    float pitch = doc["pitch"];
    float roll = doc["roll"];

    Serial.print("p=");
    Serial.println(pitch);
    Serial.print("r=");
    Serial.println(roll);

    // Blink LED to indicate we've received serial data
    //b = !b;
    b = pitch > 0;
    digitalWrite(D5, b);

    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
    // print the number of seconds since reset:
    lcd.print(millis() / 1000);
  }
}
