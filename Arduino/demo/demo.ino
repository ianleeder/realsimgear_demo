#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
StaticJsonDocument<200> doc;

void setup() {
  Serial.begin(9600, SERIAL_8N1);
  Serial.setTimeout(200);

  Wire.begin(D2, D1);
  lcd.begin(16,2);
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Pitch:");
  lcd.setCursor(0,1);
  lcd.print("Roll:");
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
        return;
    }

    float pitch = doc["pitch"];
    float roll = doc["roll"];

    Serial.print("p=");
    Serial.println(pitch);
    Serial.print("r=");
    Serial.println(roll);

    char str[10];
    
    sprintf(str, "%+.1f  ", pitch);
    lcd.setCursor(7,0);
    lcd.print(str);

    sprintf(str, "%+.1f  ", roll);
    lcd.setCursor(7,1);
    lcd.print(str);
  }
}
