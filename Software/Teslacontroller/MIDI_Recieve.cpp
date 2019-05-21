//MIDI_Recieve.cpp
//
//MIDI Auslesefunktion
//Verantwortlich für das Auslesen und Weiterleiten des MIDI Eingangs. Soll schon mal alle Befehle mit denen der Code eh nix anfangen kann verwerfen.
//
//Author: Michael Loose
//FHWS Fakultät Elektrotechnik
//
//Erstellung: 5/2019
//Stand: 15/5/2019
//////////////////////////////////////////////////////////////////////

#include "MIDI_Recieve.hpp"

void pollMidiIn(void) {

  do {
    if (Serial3.available()) {
      byte byte0 = Serial3.read(); //Status/Channel byte auslesen
      byte byte1 = Serial3.read(); //Datenbyte 1 auslesen
      byte byte2 = Serial3.read(); //Datenbyte 2 auslesen
    }


  }  while (Serial3.available() > 2); //Wenn mindestens 3 byte im Puffer sind




}
