//MIDI_Read.cpp
//
//Schnittstelle zur Übergabe von MIDI Befahlen an den DSP
//
//
//Author: Mark Philipp Richter
//FHWS Fakultät Elektrotechnik
//
//Erstellung: 5/2019
//Stand: 22/05/2019
//////////////////////////////////////////////////////////////////////

#include"MIDI_Read.hpp"


byte freqAdress[] = {0x00, 0x04, 0x08, 0x0c};  //Registeradressen für die Frequenz des jeweiligen Outputs
byte dutyAdress[] = {0x01, 0x05, 0x09, 0x0d};  //Registeradressen für das Tastverhältnis jeweiligen Outputs
byte outAdress[] = {0x02, 0x06, 0x0a, 0x0e};   //Registeradressen füs Aktivieren/deaktivieren der jeweiligen Outputs
// MIDI Wert zu Frequenz
uint32_t frequency[128] = {1604, 1699, 1800, 1907, 2021, 2141, 2268, 2403, 2546, 2697, 2858, 3028, 3208, 3398, 3600, 3814, 4041, 4282, 4536, 4806, 5092, 5394, 5715, 6055, 6415, 6797, 7201, 7629, 8083, 8563, 9072, 9612, 10183, 10789, 11431, 12110, 12830, 13593, 14401, 15258, 16165, 17126, 18144, 19224, 20370, 21578, 22861, 24220, 25660, 27186, 28803, 30515, 32329, 34252, 36289, 38448, 40734, 43156, 45723, 48441, 51321, 54374, 57607, 61032, 64660, 68505, 72581, 76896, 81467, 86312, 91445, 96883, 102643, 108746, 115214, 122065, 129323, 137012, 145160, 153791, 162935, 172625, 182889, 193759, 205294, 217492, 230425, 244126, 258649, 274028, 290316, 307582, 325880, 345244, 365778, 387536, 410570, 434984, 460849, 488252, 517297, 548056, 580631, 615165, 651742, 690505, 731557, 775055, 821140, 869969, 921698, 976521, 1034578, 1096094, 1161280, 1230329, 1303485, 1380992, 1463113, 1550110, 1642297, 1739955, 1843414, 1953025, 2069155, 2192205};

byte playingTone[4];
uint32_t playingDutyCycle[4] = {0x400000, 0x400000, 0x400000, 0x400000};

void readMidi(byte byte0, byte byte1, byte byte2) {
  byte channel = byte0 & 0x0f;
  
  for (int i = 0; i < 3; i++) {
  
    byte channel = byte0 & 0x0f;
    
    if( (getSettings().source[i] > 1) || (getSettings().source[i] < 10)){
    byte target = getSettings().source[i]-2;
        
      if(target == channel){
        byte newTarget = (byte0 & 0xF0) | i;
        
        
        outputMidiToDSP(newTarget, byte1, byte2);
      }
      
    }

  }



}



void outputMidiToDSP(byte byte0, byte byte1, byte byte2) {
  
  byte channel = byte0 & 0x0f;
  if (channel < 4) { //Sichergehen dass kein Oszillator angesprochen wird der physikalisch nicht existiert.
    if ((byte0 & 0xF0) == MIDI_CMD_NOTE_ON) { //Note on
      playingTone[channel] = 0b01111111 & byte1;
      noteOn(channel, frequency[playingTone[channel]] , playingDutyCycle[channel]);
      //Serial.print("AN");
    }

    if ((byte0 & 0xF0) == MIDI_CMD_NOTE_OFF) { //Note off
      if (playingTone[channel] == (0b01111111 & byte1)) {
        noteOff(channel);
        //Serial.print("AUS");
      }


    }

    if ((byte0 & 0xB0) && ((byte1 == 0x78) || (byte1 == 0x7B))) { // All Sounds off
      noteOff(channel);
    }
  }
}
void noteOn(byte output, uint32_t freqIn, uint32_t dutyIn) {

  // uint32_t convDuty = map(dutyIn, 0, 100, 0x00, 0x800000);
  // uint32_t convFreq = map(freqIn, 0, 24000, 0x00, 0x400000);
  uint32_t enableOutput = 0x00800000;

  //Erzeugen des Byte Array für die Übertragung
  //Hier wird der Burst Mode verwendet, also die Zieladresse nach jeder Übertragung um 1 erhöht.
  byte transmitData[14];
  transmitData[0] = 0x00; //Am Anfang 1 Byte mit Nullen
  transmitData[1] = freqAdress[output]; //Danach die Startadresse
  //Frequenz
  transmitData[2] = (freqIn >> 24) & 0xFF;
  transmitData[3] = (freqIn >> 16) & 0xFF;
  transmitData[4] = (freqIn >> 8) & 0xFF;
  transmitData[5] = freqIn & 0xFF;
  //Tastverhältnis
  transmitData[6] = (dutyIn >> 24) & 0xFF;
  transmitData[7] = (dutyIn >> 16) & 0xFF;
  transmitData[8] = (dutyIn >> 8) & 0xFF;
  transmitData[9] = dutyIn & 0xFF;
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
