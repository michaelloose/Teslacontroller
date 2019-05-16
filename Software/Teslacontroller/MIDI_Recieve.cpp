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

  if (Serial3.available()) {

    int incomingByte = Serial3.read();
    if (incomingByte & 0b10000000) {
      Serial.println(incomingByte, HEX);
    }
  }
}
