#include<Wire.h>

#define pcf8575adress 0x20

volatile bool buttonPressed = 0;

bool encplus = true;
bool encminus = true;

void pcf8575ISR() {

  buttonPressed = 1;

}


void setup() {
  Wire.begin();
  Serial.begin(9600);

  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), pcf8575ISR, FALLING);
  Wire.requestFrom(pcf8575adress, 1);


}

void loop() {
//  Wire.requestFrom(pcf8575adress, 1);
//  byte incomingByte = Wire.read();
//  Serial.println(incomingByte, BIN);
  if (buttonPressed) {
    Wire.requestFrom(pcf8575adress, 1);
    byte incomingByte = Wire.read();
    Serial.println(incomingByte, BIN);

    if (incomingByte == 0b00000011) {
      encplus = true;
      encminus = true;
    }
    if ((incomingByte == 0b00000001) && encminus) {
      Serial.println("Enc+");
      encplus = false;
    }
    if ((incomingByte == 0b00000010) && encplus ) {
      Serial.println("Enc-");
      encminus = false;
    }
    if (incomingByte & 0b00000100) Serial.println("Enc Click");
    if (incomingByte & 0b00001000) Serial.println("Button 1");
    if (incomingByte & 0b00010000) Serial.println("Button 2");
    if (incomingByte & 0b00100000) Serial.println("Button 3");
    if (incomingByte & 0b01000000) Serial.println("Button 4");
    if (incomingByte & 0b10000000) Serial.println("Home");
    buttonPressed = 0;
  }

}
