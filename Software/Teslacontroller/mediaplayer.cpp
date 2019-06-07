//Teslacontroller.ino
//
//Mediaplayer für den Teslacontroller
//
//
//Author: Mark Philipp Richter
//FHWS Fakultät Elektrotechnik
//
//Erstellung: 4/2019
//Stand: 22/05/2019
//////////////////////////////////////////////////////////////////////

//SYNTAX AUSGABE: readMidi(byte0, byte1, byte2)
#define BAUDRATE 250000

#include "mediaplayer.hpp"

byte maxfiles = 100;
String fileList[100];  // Muss maxfiles entsprechen
byte currentFile = 0;
boolean playing = false;
int counter = 0;
boolean fileSelected = 0;
boolean initialized = 0;

SdFat SD;
MD_MIDIFile SMF;
File root;
const int mSDcard = 53; // Entsprechenden Pin einfügen (20-49)
const int SDcard = 9; // Entsprechenden Pin einfügen   (20-49)
// MISO 50; MOSI 51; SCK 52





int initializeSD(void) {      // SDkarte Initialisieren und Dateiliste erstellen
  int fehlercode = -1; // -1: kein Fehler; 1: keine SDkarte; 2: Keine Dateien
  int SDslot = SDcard;
  counter = 0;

  if (!SD.begin(SDslot)) {
    SDslot = mSDcard;

    if (!SD.begin(SDslot)) {
      fehlercode = 1;
    }
  }
readData:


  fileList[0] = "NO DATA";


  root = SD.open("/");        //root-Verzeichnis als Standard

  createfileList(root);

  Serial.println("Ab jetzt fileList[] \n");      // Kontrolle ob richtig eingelesen wurde
  for (int i = 0; i < counter; i++) {
    Serial.print(i + 1);
    Serial.print(". \t");
    Serial.println(fileList[i]);
  }
  if ((counter == 0)  && (SDslot == SDcard)) {    // MicroSD prüfen falls SD keine .mid enthält
    Serial.println(fileList[counter]);

    SDslot = mSDcard;


    if (!SD.begin(SDslot)) {                    //MIcroSD nicht vorhanden -> Fehler 2
      fehlercode = 2;
    }

    goto readData;
  }

  if ((counter == 0)  && (SDslot == mSDcard)) {     // Wenn (auch) nix auf der MicroSD ist: Fehler 2
    Serial.println(fileList[counter]);

    fehlercode = 2;

  }
  // Initialize MIDIFile
  SMF.begin(&SD);
  SMF.setMidiHandler(midiCallback);
  SMF.setSysexHandler(sysexCallback);
  SMF.close();  //Datei schließen, falls eine geöffnet war


  SPI.end();
  initialized = 1;
  resetPitchBend();

  return fehlercode; //-1 bedeutet fehlerfrei initialisiert, alles andere ist ein Fehlercode

}

//Wird im Durchlauf aufgerufen. Setzt nur das Playing Bit
void playFile(void) {
  playing = true;
  SMF.pause(false);
  refreshScreen();
}

void pauseFile(void) {
  if (playing) {
    playing = false;
    midiSilence();
    SMF.pause(true);
    refreshScreen();

  }
}

void playPauseFile (void) {
  if (playing) {
    pauseFile();
  }
  else  {
    playFile();
  }
}

void resetFile(void) {
  pauseFile();
  SMF.close();
  fileSelected = 1;
  loadMidiFile();

}

void selectFile(byte number) {
  if (!initialized) {
    initializeSD();
  }
  if ((number < counter + 1) && initialized) {
    currentFile = number;
    resetFile();
    refreshScreen();

  }
}

void loadMidiFile(void) {       //einmalig bei Dateiauswahl ausführen


  if ((playing == true) && (fileSelected == 1)) {
    fileSelected = 0;

    int  err;

    Serial.print("\nFile: ");
    Serial.print(fileList[currentFile]);
    SMF.setFilename(fileList[currentFile].c_str());
    err = SMF.load();

    if (err != -1)
    {
      Serial.print("\nSMF load Error ");
      Serial.print(err);

      //delay(2000);
    }
  }

}


bool playMidiFile(void) {           // Midi Datei abspielen
  if (!getUserInput()) {
    if (playing) {


      if (!SMF.isEOF())
      {

        if (SMF.getNextEvent())
          ;

      }
      else {
        SMF.close();
        midiSilence();
        playing = false;

        //    currentFile++;
        fileSelected = 1;

        SPI.end();
        return 1;               //Rückgabe zur Bildschirmaktualisierung beim Ende der Datei
      }

      SPI.end();
      return 0;
    }
  }
  return 0;


}




void createfileList(File dir) {    //Dateinamen ins Array schreiben

  for (int j = 0; j < maxfiles; j++) {

    File entry =  dir.openNextFile();

    if (! entry) {
      // no more files
      break;
    }

    if (!entry.isDirectory()) {

      if ( isMidi(fileNameAsString(entry).c_str()) ) {

        fileList[counter] = fileNameAsString(entry);

        counter++;

      }

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

void printFileList (void) {         // Druckt Songliste aus ;)
  Serial.begin(19200);
  byte underline = 0b10000000;
  byte doubleHeight = 0b00010000;
  byte doubleWidth = 0b00100000;
  byte resetPrintMode = 0b00000000;
  int setPrintMode[] = {27, 33};
  int lineFeed = 10;
  char number[3];

  if (!initialized) {         
    initializeSD();
  }
  for (int i = 0; i < 2; i++)
  {
    Serial.write(setPrintMode[i]);
  }
  Serial.write(doubleHeight | doubleWidth);                   // Ungetestet

  Serial.write("Song List:");
  Serial.write(resetPrintMode);

  for (int i = 0; i < counter + 1; i++) {
    itoa(i, number, 10);
    Serial.write(number);
    Serial.write(".");
    Serial.write('\t');
    Serial.write(fileList[i].c_str());
    Serial.write(lineFeed);

  }
  Serial.begin(BAUDRATE);
}


bool isMidi(char* filename) {
  int8_t len = strlen(filename);
  bool result;
  if (  strstr(strlwr(filename + (len - 4)), ".MID")
        || strstr(strlwr(filename + (len - 4)), ".mid")
        || strstr(strlwr(filename + (len - 4)), ".tmf")
        || strstr(strlwr(filename + (len - 4)), ".TMF")
        // and anything else you want
     ) {
    result = true;
  } else {
    result = false;
  }
  return result;
}
// Ab hier MIDI Funktionen

void midiCallback(midi_event *pev)
// Called by the MIDIFile library when a file event needs to be processed
// thru the midi communications interface.
// This callback is set up in the setup() function.
{
  if ((pev->channel) < 4) {

    if (false) {
      Serial.print("\n");
      Serial.print(" T");
      Serial.print(pev->track);
      Serial.print(":  Ch ");
      Serial.print(pev->channel + 1);
      Serial.print(" Data ");
      for (uint8_t i = 0; i < pev->size; i++)
      {
        Serial.print(pev->data[i], HEX);
        Serial.print(' ');
      }
    }
    readMidi(pev->data[0] | pev->channel + 4, pev->data[1], pev->data[2]);
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
  for (uint8_t i = 0; i < pev->size; i++)
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

int getNumberOfLoadedFiles(void) {
  return counter;
}

void setCurrentFile(byte i) {
  currentFile = i;
  Serial.print("\n");
  Serial.println(fileList[currentFile]);
  Serial.println(currentFile);
}
bool getPlayingState(void) {
  return playing;
}

void setFileSelected(void) {
  fileSelected = 1;
}
