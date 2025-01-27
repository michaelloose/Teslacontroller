/*
Setup Funktion für den EEPROM des Teslacontrollers: Programmiert alle nötigen Werte in den EEPROM
*/
#include <EEPROM.h>
uint32_t DutyCycle[128] = {68853, 74138, 78952, 83452, 87767, 92009, 96268, 100620, 105125, 109833, 114780, 119996, 125504, 131320, 137455, 143918, 150713, 157846, 165320, 173136, 181300, 189814, 198685, 207919, 217525, 227513, 237895, 248685, 259899, 271555, 283670, 296266, 309361, 322979, 337140, 351865, 367177, 383094, 399635, 416818, 434657, 453164, 472349, 492216, 512770, 534006, 555920, 578500, 601731, 625592, 650056, 675093, 700666, 726733, 753249, 780162, 807415, 834949, 862700, 890601, 918583, 946575, 974506, 1002304, 1029900, 1057227, 1084223, 1110829, 1136997, 1162686, 1187868, 1212526, 1236661, 1260292, 1283458, 1306222, 1328675, 1350936, 1373161, 1395540, 1418306, 1441736, 1466156, 1491949, 1519552, 1549469, 1582273, 1618609, 1659203, 1704867, 1756506, 1815123, 1881825, 1957833, 2044487, 2143255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
byte zero = 0x00;
int coilType = 1;

int eeStartAddress = 128;

int eeSettingsStartAddress = 0;


void setup() {

  eeStartAddress = eeStartAddress + coilType * 512;
  // initialize serial and wait for port to open:
  Serial.begin(250000);


  for (int i = 0; i < 128; i++) {
    int eeAddress = eeSettingsStartAddress + i;
    EEPROM.put(eeAddress, zero);
  }

  for (int i = 0; i < 128; i++) {
    int eeAddress = eeStartAddress + (4 * i);
    EEPROM.put(eeAddress, DutyCycle[i]);
  }

  
  Serial.write("WRITE COMPLETE");
}


void loop() {

}
