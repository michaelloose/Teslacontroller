/*
   EEPROM Read

   Reads the value of each byte of the EEPROM and prints it
   to the computer.
   This example code is in the public domain.
*/
#include <EEPROM.h>

int eeStartAddress = 512;

void setup() {
  // initialize serial and wait for port to open:
  Serial.begin(9600);


  for (int i = 0; i < 128; i++) {
    int eeAddress = eeStartAddress + (4 * i);
    EEPROM.put(eeAddress, 0x200000);
  }
Serial.write("RESET COMPLETE");
}


void loop() {

}
