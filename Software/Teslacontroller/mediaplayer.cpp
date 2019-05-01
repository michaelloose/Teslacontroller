//Teslacontroller.ino
//
//Main File für den Teslacontroller
//
//
//Author: Mark Phillipp Richter
//FHWS Fakultät Elektrotechnik
//
//Erstellung: 4/2019
//Stand: 01/05/2019
//////////////////////////////////////////////////////////////////////

//SYNTAX AUSGABE: readMidi(byte0, byte1, byte2)

#include "mediaplayer.hpp"

String fileList[64];
byte currentFile = 0;
boolean playing = false;
int counter = 0;

const int mSDcard = 40; // Entsprechenden Pin einfügen (20-49)
const int SDcard = 41; // Entsprechenden Pin einfügen   (20-49)

int initializeSD(void) {      // SDkarte Initialisieren und Dateiliste erstellen
  byte fehlercode = -1; // -1: kein Fehler; 1: keine SDkarte; 2: Keine Dateien
  int SDslot = SDcard;
  
  if (!SD.begin(SDslot)) {
     SDslot = mSDcard;
  }
  if (!SD.begin(SDslot)) {
    fehlercode = 1;
  }

  fileList[0] = "File 1";
  fileList[1] = "File 2";
  fileList[2] = "File 3";
  fileList[3] = "File 4";
  fileList[4] = "File 5";
  fileList[5] = "File 6";
  fileList[6] = "File 7";
  fileList[7] = "abcdefghijklmnopqrstuvwxyz0123456789";


  root = SD.open("/");        //root-Verzeichnis als Standard
  
  printDirectory(root, 0);


  return fehlercode; //-1 bedeutet fehlerfrei initialisiert, alles andere ist ein Fehlercode

}

//Wird im Durchlauf aufgerufen. Setzt nur das Playing Bit 
void playFile(void) {
  playing = true;

}

void pauseFile(void) {
  playing = false;

}

void pollMediaPlayer(void){
  //Wenn das playing Bit gesetzt ist läuft diese Schleife mit der Main Loop mit
  if(playing){

  //Hierein gehört der ganze Code zum Abspielen!
    
  }
}

void printDirectory(File dir, int numTabs) {    //Dateinamen ins Array schreiben

  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    if (!entry.isDirectory()){
    
    Serial.print(entry.name());
    fileList[counter]=entry.name();
//      Serial.print("\t\t");
//      Serial.println(entry.size(), DEC);
    Serial.print('\n');
    counter++;
    }
    entry.close();
    
    }
  }




//Ab hier sind nur noch die Getter/Settermethoden
String getFileList(byte k) {
  return fileList[k];
}
byte getCurrentFile(void) {
  return currentFile;
}
void setCurrentFile(byte i) {
  currentFile = i;
}
bool getPlayingState(void) {
  return playing;
}
