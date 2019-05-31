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

byte pitchBendRange = 2; //In Halbtönen
int bend[4] = {0x2000, 0x2000, 0x2000, 0x2000};

byte freqAdress[] = {0x00, 0x04, 0x08, 0x0c};  //Registeradressen für die Frequenz des jeweiligen Outputs
byte dutyAdress[] = {0x01, 0x05, 0x09, 0x0d};  //Registeradressen für das Tastverhältnis jeweiligen Outputs
byte outAdress[] = {0x02, 0x06, 0x0a, 0x0e};   //Registeradressen füs Aktivieren/deaktivieren der jeweiligen Outputs
// MIDI Wert zu Frequenz
//Erstellt mit Matlab Skript: Einheit ist Frequenz/48kHz (DSP Samplingrate) Position ist auf das MIDI Frequenzbyte angepasst
const PROGMEM uint32_t frequency[128] = {1429, 1514, 1604, 1699, 1800, 1907, 2021, 2141, 2268, 2403, 2546, 2697, 2858, 3028, 3208, 3398, 3600, 3815, 4041, 4282, 4536, 4806, 5092, 5395, 5715, 6055, 6415, 6797, 7201, 7629, 8083, 8563, 9072, 9612, 10184, 10789, 11431, 12110, 12830, 13593, 14402, 15258, 16165, 17127, 18145, 19224, 20367, 21578, 22861, 24221, 25661, 27187, 28803, 30516, 32331, 34253, 36290, 38448, 40734, 43156, 45722, 48441, 51322, 54373, 57607, 61032, 64661, 68506, 72580, 76896, 81468, 86312, 91445, 96882, 102643, 108747, 115213, 122064, 129322, 137012, 145160, 153791, 162936, 172625, 182890, 193765, 205287, 217494, 230426, 244128, 258645, 274025, 290319, 307582, 325872, 345249, 365779, 387529, 410573, 434987, 460853, 488256, 517290, 548049, 580638, 615165, 651744, 690499, 731558, 775059, 821146, 869974, 921705, 976513, 1034579, 1096099, 1161276, 1230329, 1303488, 1380998, 1463116, 1550118, 1642292, 1739948, 1843411, 1953026, 2069159, 2192197};


byte playingTone[4];
uint32_t playingDutyCycle[4];



//Die folgenden Funktionen sind nur dazu gut die komische Ausgabe der Arduino MIDI Library wieder in normale MIDI Commands zurückzuändern die in der folgenden Klasse ausgewertet werden sollen.
//Das sollte irgendwann mal geändert werden, also im idealfall die Library ändern oder selbst eine schreiben

void readMidiInputOn(byte byte0, byte byte1, byte byte2) {
  byte0--; //Wird von der Library 1 Indiziert übergeben, wir arbeiten aber hier mit einer 0- Indizierung

  if (byte0 < 4) { //Nur Channels von 0-3 sollen verwendet werden
    byte0 = byte0 | MIDI_CMD_NOTE_ON; //Note On Information wird zum Byte hinzugefügt
    readMidi(byte0, byte1, byte2);
    
  }
  
}
void readMidiInputOff(byte byte0, byte byte1, byte byte2) {
  byte0--; //Wird von der Library 1 Indiziert übergeben, wir arbeiten aber hier mit einer 0- Indizierung

  if (byte0 < 4) { //Nur Channels von 0-3 sollen verwendet werden
    byte0 = byte0 | MIDI_CMD_NOTE_OFF; //Note Off Information wird zum Byte hinzugefügt
    readMidi(byte0, byte1, byte2);
    
  }
    
}
void readMidiPitchBend(byte byte0, int bend) {
  byte0--; //Wird von der Library 1 Indiziert übergeben, wir arbeiten aber hier mit einer 0- Indizierung

  if (byte0 < 4) { //Nur Channels von 0-3 sollen verwendet werden
    byte0 = byte0 | MIDI_CMD_PITCH_BEND; //Note Off Information wird zum Byte hinzugefügt
    bend += 8192;
    byte byte1 = bend & 0xFF;
    byte byte2 = (bend >> 8) & 0xFF;
    readMidi(byte0, byte1, byte2); 
  }
  
}

void readMidiSysex(byte* byteEv, unsigned sizeEv){
  Serial.println("Sysex");
  for (int i=0; i< (sizeEv+1); i++){
  Serial.println(byteEv[i]);
  }
}

//Übernimmt das Mapping der einzelnen MIDI Channels auf die gewählten CoilTypes. Channels 0-3 kommen vom MIDI Input, Channels 4-7 sind die Channels 0-3 vom Player
//Wird mit normalen MIDI Bytes aufgerufen
void readMidi(byte byte0, byte byte1, byte byte2) {
  byte channel = byte0 & 0x0f;

  for (int i = 0; i < 4; i++) {

    byte channel = byte0 & 0x0f;

    if ( (getSettings().source[i] > 1) || (getSettings().source[i] < 10)) {
      byte target = getSettings().source[i] - 2;

      if (target == channel) {
        byte newTarget = (byte0 & 0xF0) | i;

        //Debugging: Kommentierung entfernen damit die MIDI Befehle über die serielle Schnittstelle ausgegeben werden
        /*
          Serial.print(byte0, HEX);
          Serial.print("  ");
          Serial.print(byte1, HEX);
          Serial.print("  ");
          Serial.println(byte2, HEX);
        */

        outputMidiToDSP(newTarget, byte1, byte2);
      }
    }
  }
}

void outputMidiToDSP(byte byte0, byte byte1, byte byte2) {
  
  byte channel = byte0 & 0x0F; //Die letzten 4 Bit enthalten die Information über den relevanten Channel
  if (channel < 4) { //Sichergehen dass kein Oszillator angesprochen wird der physikalisch nicht existiert.
    if ((byte0 & 0xF0) == MIDI_CMD_NOTE_ON) { //Note on
      playingTone[channel] = 0b01111111 & byte1;

      //Liest das gewünschte Tastverhältnis aus dem EEprom aus
      int eeReadAddress = eeDCAddress + (getSettings().coilType[channel] * 512) + (4 * playingTone[channel]);
      EEPROM.get(eeReadAddress, playingDutyCycle[channel] );

      //Bestimmt das Tastverhältnis aus einer konstanten On Time
      //War der alte Ansatz
      //playingDutyCycle[channel] =  pgm_read_dword(&frequency[playingTone[channel]])* getSettings().coilType[channel];

      
        
       if (bend[channel] != 0x2000){    

        if (bend[channel] < 0x2000) {
          uint32_t newFrequency = getFrequency(playingTone[channel])-(((getFrequency(playingTone[channel])-getFrequency(playingTone[channel]-pitchBendRange))*(0x2000-bend[channel]))/0x2000);
          
          noteOn(channel, newFrequency , playingDutyCycle[channel]);
        }

        else if (bend[channel] < 0x4000) {
          uint32_t newFrequency = getFrequency(playingTone[channel])+(((getFrequency(playingTone[channel]+pitchBendRange)-getFrequency(playingTone[channel]))*(bend[channel]-0x2000))/0x2000);
          noteOn(channel, newFrequency , playingDutyCycle[channel]);
        }
       }
      else{  
      noteOn(channel, getFrequency(playingTone[channel]) , playingDutyCycle[channel]);
      } //Serial.print("AN");
    }

    if ((byte0 & 0xF0) == MIDI_CMD_NOTE_OFF) { //Note off
      if (playingTone[channel] == (0b01111111 & byte1)) {
        noteOff(channel);
        playingTone[channel] = 0;
        //Serial.println("AUS");
      }


    }

    if ((byte0 & 0xF0) == MIDI_CMD_PITCH_BEND) { //Pitch Bend
      int tempBend = (byte1 & 0xFF) | ((byte2  & 0xFF) << 8);
       if(tempBend < 0x4000){
        
        bend[channel] = tempBend;
       }
      if (playingTone[channel] != 0) {         // Ändern der Frequenz nach Pitchwert um maximal pitchBendRange Halbtöne
        
        if (bend[channel] < 0x2000) {
          uint32_t newFrequency = getFrequency(playingTone[channel])-(((getFrequency(playingTone[channel])-getFrequency(playingTone[channel]-pitchBendRange))*(0x2000-bend[channel]))/0x2000);
          
          noteOn(channel, newFrequency , playingDutyCycle[channel]);
        }

        else if (bend[channel] < 0x4000) {
          uint32_t newFrequency = getFrequency(playingTone[channel])+(((getFrequency(playingTone[channel]+pitchBendRange)-getFrequency(playingTone[channel]))*(bend[channel]-0x2000))/0x2000);
          noteOn(channel, newFrequency , playingDutyCycle[channel]);
        
        
        
        }
      }
    }
  }


  if (((byte0 & 0xF0) == MIDI_CMD_CONTROLLER_CHANGE) && ((byte1 == 0x78) || (byte1 == 0x7B))) { // All Sounds off
    noteOff(channel);
    playingTone[channel] = 0;
//    Serial.print('\n');
//    Serial.print("PANIKAUS");
//    Serial.print("Byte1: ");
//    Serial.print(byte1, HEX);
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
uint32_t getFrequency(byte midiValue) {
  return pgm_read_dword(&frequency[midiValue]);
}

void resetPitchBend(void){
  for (int i=0; i<4; i++){
  bend[i] = 0x2000;
  }
}
