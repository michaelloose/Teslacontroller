//Mainboard_DSP_Commtest
//
//Test für den DSP: Erzeugt Rechtecke auf dem Kanal 0
//Funktionen noteOn und noteOff sind zur Verwendung im fertigen Code vorgesehen.
//
//Author: Michael Loose
//FHWS Fakultät Elektrotechnik
//
//Erstellung: 5/2019
//Stand: 21/05/2019
//////////////////////////////////////////////////////////////////////

#define DSP_Adress 0x34

#include <Wire.h>

byte freqAdress[] = {0x00, 0x04, 0x08, 0x0c};  //Registeradressen für die Frequenz des jeweiligen Outputs
byte dutyAdress[] = {0x01, 0x05, 0x09, 0x0d};  //Registeradressen für das Tastverhältnis jeweiligen Outputs
byte outAdress[] = {0x02, 0x06, 0x0a, 0x0e};   //Registeradressen füs Aktivieren/deaktivieren der jeweiligen Outputs






void noteOn(byte output, int freqIn, int dutyIn) {

  uint32_t convDuty = map(dutyIn, 0, 100, 0x00, 0x800000);
  uint32_t convFreq = map(freqIn, 0, 24000, 0x00, 0x400000);
  uint32_t enableOutput = 0x00800000;

  //Erzeugen des Byte Array für die Übertragung
  //Hier wird der Burst Mode verwendet, also die Zieladresse nach jeder Übertragung um 1 erhöht.
  byte transmitData[14];
  transmitData[0] = 0x00; //Am Anfang 1 Byte mit Nullen
  transmitData[1] = freqAdress[output]; //Danach die Startadresse
  //Frequenz
  transmitData[2] = (convFreq >> 24) & 0xFF;
  transmitData[3] = (convFreq >> 16) & 0xFF;
  transmitData[4] = (convFreq >> 8) & 0xFF;
  transmitData[5] = convFreq & 0xFF;
  //Tastverhältnis
  transmitData[6] = (convDuty >> 24) & 0xFF;
  transmitData[7] = (convDuty >> 16) & 0xFF;
  transmitData[8] = (convDuty >> 8) & 0xFF;
  transmitData[9] = convDuty & 0xFF;
  //enable
  //Feste Sequenz um den Ausgang zu aktivieren
  transmitData[10] = 0x00;
  transmitData[11] = 0x80;
  transmitData[12] = 0x00;
  transmitData[13] = 0x00;

  //Alles über i2c an den DSP Übertragen
  Wire.beginTransmission(DSP_Adress);
  Wire.write(transmitData, 14);
  Wire.endTransmission(DSP_Adress);


}

void noteOff(byte output) {
  byte transmitData[6];
  transmitData[0] = 0x00; //Am Anfang 1 Byte mit Nullen
  transmitData[1] = outAdress[output]; //Danach die Registeradresse

  //Feste Sequenz um den Ausgang zu deaktivieren
  transmitData[2] = 0x00;
  transmitData[3] = 0x00;
  transmitData[4] = 0x00;
  transmitData[5] = 0x00;
  Wire.beginTransmission(DSP_Adress);
  Wire.write(transmitData, 6);
  Wire.endTransmission(DSP_Adress);

}

void setup() {
  Wire.begin(); // join i2c bus (address optional for master)

}



void loop() {


  for (int i = 1; i < 5; i++) {
    noteOn(0, 110 * i, 50);
    delay(500);
    noteOff(0);
    delay(500);
  }

}
