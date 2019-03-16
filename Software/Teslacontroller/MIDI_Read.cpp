#include"MIDI_Read.hpp"

// MIDI Wert zu Frequenz
//erster Versuch, gelöst mit Array
//float frequency[128] = {8.176, 8.662, 9.177, 9.723, 10.301, 10.913, 11.562, 12.25, 12.978, 13.75, 14.568, 15.434, 16.352, 17.324, 18.354, 19.445, 20.601, 21.826, 23.124, 24.499, 25.956, 27.5, 29.135, 30.867, 32.703, 34.648, 36.708, 38.89, 41.203, 43.653, 46.249, 48.999, 51.913, 55, 58.27, 61.735, 65.406, 69.295, 73.416, 77.781, 82.406, 87.307, 92.499, 97.998, 103.82, 110, 116.56, 123.47, 130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 184.99, 195.99, 207.65, 220, 233.08, 246.94, 261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 391.99, 415.31, 440, 466.16, 493.88, 523.25, 554.37, 587.33, 622.25, 659.26, 698.46, 739.99, 783.99, 830.61, 880, 932.32, 987.77, 1046.5, 1108.7, 1174.7, 1244.5, 1318.5, 1396.9, 1480, 1568, 1661.2, 1760, 1864.7, 1975.5, 2093, 2217.5, 2349.3, 2489, 2637, 2793.8, 2960, 3136, 3322.4, 3520, 3729.3, 3951.1, 4186, 4434.9, 4698.6, 4978, 5274, 5587.7, 5919.9, 6271.9, 6644.9, 7040, 7458.6, 7902.1, 8372, 8869.8, 9397.3, 9956.1, 10548.1, 11175.3, 11839.8, 12543.9 };
//zweiter Versuch: Direkte Berechnung im System
float frequency(int n) {

  float f = pow(2, ((n - 69) / 12) * 440);

  return f;
}

int readMidi(void) {
  static byte note;
  static byte lastCommand = MIDI_IGNORE;
  static byte state;
  static byte lastByte;


  byte incomingByte = 0x00; //DUMMY!! Wie fragt man den Seriellen Eingang des XMCs ab?

  // Abfrage Command Byte
  if (incomingByte & 0b10000000) {
    if (respondAllChannels || (incomingByte & 0x0F) == responseChannel) { // Abfrage korrekter Kanal
      lastCommand = incomingByte & 0xF0;
    }
    else { //Befehl ignorieren
      lastCommand = MIDI_IGNORE;
    }

    state = MIDI_STATE_BYTE1; // Reset our state to byte1.
  } else if (state == MIDI_STATE_BYTE1) { // process first data byte
    if ( lastCommand == MIDI_CMD_NOTE_OFF )
    { // if we received a "note off", make sure that is what is currently playing
      if (note == incomingByte) return -1;
      state = MIDI_STATE_BYTE2; // expect to receive a velocity byte
    } else if ( lastCommand == MIDI_CMD_NOTE_ON ) { // if we received a "note on", we wait for the note (databyte)
      lastByte = incomingByte;  // save the current note
      state = MIDI_STATE_BYTE2; // expect to receive a velocity byte
    }
    // implement whatever further commands you want here
  } else { // process second data byte
    if (lastCommand == MIDI_CMD_NOTE_ON) {
      if (incomingByte != 0) {
        note = lastByte;
        return (frequency(note));
      } else if (note == lastByte) {
        return -1;
      }
    }
    state = MIDI_STATE_BYTE1; // message data complete
    // This should be changed for SysEx
  }

}
