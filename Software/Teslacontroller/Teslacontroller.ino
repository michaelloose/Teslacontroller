//Teslacontroller.ino
//
//Main File für den Teslacontroller
//
//
//Author: Michael Loose
//FHWS Fakultät Elektrotechnik
//
//Erstellung: 3/2019
//Stand: 01/04/2019
//////////////////////////////////////////////////////////////////////

//Bei der Benennung von Variablen wird ein striktes Schema verwendet:
//erster Buchstabe ist immer klein, jedes neue Wort beginnt mit einem Grossbuchstaben.
//Gettermethoden bekommen die Vorsilbe get, Settermethoden set
//lokale Variablen, welche sich von globalen ableiten bekommen die vorsilbe loc


#include "lib/SPI/src/SPI.h"
#include "ui.hpp"
#include "lib/MIDI/src/MIDI.h"

#define BAUDRATE 250000   //Auch in mediaplayer.cpp anpassen!!

//MIDI Bibliothek: Schnittstelle deklarieren
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI3);

void setup(void) {
  
  // Initialisierung der seriellen Schnittstellen
  Serial.begin(BAUDRATE); //Debugging Schnittstelle

  //Display initialisieren und Startbild anzeigen
  initialiseDisplay();
  loadSettings();
  printStartScreen();




  // PCF8575 für Benutzereingaben initialisieren
  initialiseButtons();

  //Default CoilType wiederherstellen
  for (int i = 0; i < 128; i++) {
    EEPROM.put(eeDCAddress + (i * 4), 0x400000);
  }


  //MIDI Bibliothek: Initialisieren
  MIDI3.setHandleNoteOn(readMidiInputOn);
  MIDI3.setHandleNoteOff(readMidiInputOff);
  MIDI3.setHandlePitchBend(readMidiPitchBend);
  MIDI3.setHandleContinue(playFile);
  MIDI3.setHandleStart(playPauseFile);
  MIDI3.setHandleStop(playPauseFile);
  MIDI3.setHandleSystemExclusive(readMidiSysex);
  MIDI3.setHandleSystemReset(resetFile);
  MIDI3.setHandleSongSelect(selectFile);
  MIDI3.begin(MIDI_CHANNEL_OMNI);

  //Etwas warten damit man den schönen Startbildschirm bewundern kann :)
  delay(3000);

  initialiseDSP();
  refreshScreen();

}

void loop(void) {
  if (getUserInput()) {
    resetUserInput();
    readUserInput();
  }
  loadMidiFile();
  if (playMidiFile()) refreshScreen();

  //MIDI Bibliothek: Auslesen
  MIDI3.read();

}
