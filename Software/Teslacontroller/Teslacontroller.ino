//Teslacontroller.ino
//
//Main File für den Teslacontroller
//
//
//Author: Michael Loose
//FHWS Fakultät Elektrotechnik
//
//Erstellung: 3/2019
//Stand: 19/03/2019
//////////////////////////////////////////////////////////////////////

//Bei der Benennung von Variablen wird ein striktes Schema verwendet:
//erster Buchstabe ist immer klein, jedes neue Wort beginnt mit einem Grossbuchstaben.
//Gettermethoden bekommen die Vorsilbe get, Settermethoden set
//lokale Variablen, welche sich von globalen ableiten bekommen die vorsilbe loc

//Die eigenen Headerfiles
#include "ui.hpp"

void setup(void) {
  //Display initialisieren und Startbild anzeigen
  initialiseDisplay();
  printStartScreen();
  //Etwas warten damit man den schönen Startbildschirm bewundern kann :)
  delay(3000);

  // Initialisierung der seriellen Schnittstellen
  Serial.begin(9600); //Debugging Schnittstelle
  Serial1.begin(31250); //MIDI Schnittstelle

  loadSettings();
  // PCF8575 für Benutzereingaben initialisieren

  initialiseButtons();

}

void loop(void) {
  taskManager.runLoop();



}
