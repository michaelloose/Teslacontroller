//Teslacontroller.ino
//
//Main File für den Teslacontroller
//
//
//Author: Michael Loose
//FHWS Fakultät Elektrotechnik
//
//Erstellung: 3/2019
//Stand: 14/03/2019
//////////////////////////////////////////////////////////////////////

//Die eigenen Headerfiles
#include "ui.hpp"


int eeAddress = 0; //Startadresse für das Auslesen vom EEPROM


bool inputState[3] = {0, 0, 0};

set settings; //Variable zum Speichern der Einstellungen erstellen

void setup(void) {
  //Display initialisieren und Startbild anzeigen
  initialiseDisplay();
  printStartScreen();
  
  // Initialisierung der seriellen Schnittstellen
  Serial.begin(9600); //Debugging Schnittstelle
  Serial1.begin(31250); //MIDI Schnittstelle
  
  // PCF8575 für Benutzereingaben initialisieren

  initialiseButtons();
  
  //Einstellungen aus dem EEprom laden
  EEPROM.get( eeAddress, settings); //Laden der Einstellungen aus dem EEprom

  //DEBUG CODE
  //Serielle ausgabe der Einstellungen
  Serial.println("Einstellungen erfolgreich geladen!");
  Serial.print("Source: ");

  for (int i = 0; i < 4; i++)
  {
    Serial.print( settings.source[i] );
    Serial.print(" ");
  }

  Serial.println("");
  Serial.print("CoilType: ");

  for (int i = 0; i < 4; i++)
  {
    Serial.print( settings.coilType[i] );
    Serial.print(" ");
  }


  //Etwas warten damit man den schönen Startbildschirm bewundern kann :)
  delay(3000);

  //Grundbild anzeigen
  printHomeScreen(settings, inputState);



}

void loop(void) {

  if (getButtonPressed()) readButtons();


}
