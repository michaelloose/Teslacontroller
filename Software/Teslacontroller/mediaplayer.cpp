//Teslacontroller.ino
//
//Mediaplayer für den Teslacontroller
//
//
//Author: Mark Philipp Richter
//FHWS Fakultät Elektrotechnik
//
//Erstellung: 4/2019
//Stand: 08/05/2019
//////////////////////////////////////////////////////////////////////

//SYNTAX AUSGABE: readMidi(byte0, byte1, byte2)

#include "mediaplayer.hpp"


String fileList[64];
byte currentFile = 0;
boolean playing = false;
int counter = 0;

SdFat SD;
MD_MIDIFile SMF;
File root;
const int mSDcard = 40; // Entsprechenden Pin einfügen (20-49)
const int SDcard = 41; // Entsprechenden Pin einfügen   (20-49)
                        // MISO 50; MOSI 51; SCK 52
int initializeSD(void) {      // SDkarte Initialisieren und Dateiliste erstellen
  byte fehlercode = -1; // -1: kein Fehler; 1: keine SDkarte; 2: Keine Dateien
  int SDslot = SDcard;
  counter = 0;
  
  if (!SD.begin(SDslot)) {
     SDslot = mSDcard;
  }
  if (!SD.begin(SDslot)) {
    fehlercode = 1;
  }

  fileList[0] = "NO DATA";
  fileList[1] = "File 2";
  fileList[2] = "File 3";
  fileList[3] = "File 4";
  fileList[4] = "File 5";
  fileList[5] = "File 6";
  fileList[6] = "File 7";
  fileList[7] = "abcdefghijklmnopqrstuvwxyz0123456789";


  root = SD.open("/");        //root-Verzeichnis als Standard
  
  printDirectory(root);

  Serial.println("Ab jetzt fileList[] \n");      // Kontrolle ob richtig eingelesen wurde
  for (int i=0; i<counter; i++){
    Serial.print(i+1);
    Serial.print(". \t");
    Serial.println(fileList[i]);
  }
  if (counter == 0){
    Serial.println(fileList[counter]);
  }

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

void printDirectory(File dir) {    //Dateinamen ins Array schreiben

  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
   

    if (!entry.isDirectory()){

    fileList[counter] = fileNameAsString(entry);
    Serial.print(fileList[counter]);
   
     
    Serial.print('\n');
    counter++;
    }
    entry.close();
    
    }
  }

String fileNameAsString(File activeFile) {         // Dateinamen als String
  char filename[20];
  String fn = "";

  activeFile.getName(filename, 50);

  for (int i = 0; i < strlen(filename); i++)
  {
    fn = fn + filename[i];
  }

  return fn;
}
// Ab hier MIDI Funktionen

void midiCallback(midi_event *pev)
// Called by the MIDIFile library when a file event needs to be processed
// thru the midi communications interface.
// This callback is set up in the setup() function.
{
#if USE_MIDI
  if ((pev->data[0] >= 0x80) && (pev->data[0] <= 0xe0))
  {
    Serial.write(pev->data[0] | pev->channel);
    Serial.write(&pev->data[1], pev->size-1);
  }
  else
    Serial.write(pev->data, pev->size);
#endif
  Serial.print("\n");
  Serial.print(millis());
  Serial.print("\tM T");
  Serial.print(pev->track);
  Serial.print(":  Ch ");
  Serial.print(pev->channel+1);
  Serial.print(" Data ");
  for (uint8_t i=0; i<pev->size; i++)
  {
  Serial.print(pev->data[i], HEX);
    Serial.print(' ');
  }
}


void sysexCallback(sysex_event *pev)
// Called by the MIDIFile library when a system Exclusive (sysex) file event needs 
// to be processed through the midi communications interface. Most sysex events cannot 
// really be processed, so we just ignore it here.
// This callback is set up in the setup() function.
{
  Serial.print("\nS T");
  Serial.print(pev->track);
  Serial.print(": Data ");
  for (uint8_t i=0; i<pev->size; i++)
  {
    Serial.print(pev->data[i], HEX);
    Serial.print(' ');
  }
}

void midiSilence(void)
// Turn everything off on every channel.
// Some midi files are badly behaved and leave notes hanging, so between songs turn
// off all the notes and sound
{
  midi_event ev;

  // All sound off
  // When All Sound Off is received all oscillators will turn off, and their volume
  // envelopes are set to zero as soon as possible.
  ev.size = 0;
  ev.data[ev.size++] = 0xb0;
  ev.data[ev.size++] = 120;
  ev.data[ev.size++] = 0;

  for (ev.channel = 0; ev.channel < 16; ev.channel++)
    midiCallback(&ev);
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
