//gui.cpp
//
//User Interface für den Teslacontroller
//Enthält alle Methoden für das Auslesen der Knöpfe und die Anzeigen auf dem Display
//
//Author: Michael Loose
//FHWS Fakultät Elektrotechnik
//
//Erstellung: 3/2019
//Stand: 01/04/2019
//////////////////////////////////////////////////////////////////////


bool inputState[3] = {0, 0, 0}; //Die Anzeigen ob ein Eingang aktiv ist. Die sollen nicht hier bleiben, sondern kommen später dahin wos mehr sinn macht.

#include "ui.hpp"

//Definition des Displayanschlusses
U8G2_SSD1322_NHD_256X64_F_4W_SW_SPI u8g2(U8G2_R2, /* clock=*/ 52, /* data=*/ 51, /* cs=*/ 49, /* dc=*/ 48, /* reset=*/ 47);


byte encpos = 0;
byte encmax = 5;
byte scrollpos = 0;

//Aktuell ausgewähltes Feld
byte cursorPositionOld = 255;
byte cursorPosition = 0;
byte cursorPosition1 = 0;
//Globaler Zwischenspeicher für verschiedene Werte
uint32_t stor;

//DutyCycle der im Menü verstellt werden kann
float dutyCycle;





//Aktuell angezeigtes Bild
//0: Grundbild
//1: Menü
//2: Player File Auswahl
//3: Coil Setup
//4: Coil Setup2 (untermenü)
//5: Coil Test/Manual Mode
//6: UtilityScreen
//7: Credits
byte currentScreen = 0;

void refreshScreen(void) {
  switch (currentScreen) {
    case 0:
      printHomeScreen(inputState);
      break;
    case 1:
      printMenuScreen();
      break;
    case 2:
      printPlayerFileScreen();
      break;
    case 3:
      printCoilSetupScreen();
      break;
    case 4:
      printCoilSetupScreen2();
      break;
    case 5:
      printCoilTestScreen();
      break;
    case 6:
      printUtilityScreen();
      break;
    case 7:
      printCreditsScreen();
      break;
    default:
      printNotificationScreen(stor);
      break;
  }
}


//Wird bei erkanntem Drehen des Encoders aufgerufen
void onEncoderChange(bool dir) {

  //GRUNDBILD
  if (currentScreen == 0) {
    set locSettings = getSettings(); //Zieht eine lokale Kopie des Structs Settings
    //Ist gerade Source ausgewählt?
    if (cursorPosition % 10 == 1) {
      //Schreibe den aktuellen Wert in das korrekte Feld.
      //Die Operation nimmt den Zehner von cursorPosition-1, also die entsprechende Ausgangsnummer



      if (!dir) {
        if (locSettings.source[((cursorPosition / 10) % 10) - 1] < 11) locSettings.source[((cursorPosition / 10) % 10) - 1]++;
        else locSettings.source[((cursorPosition / 10) % 10) - 1] = 0;
      }
      else {
        if (locSettings.source[((cursorPosition / 10) % 10) - 1] > 0) locSettings.source[((cursorPosition / 10) % 10) - 1]--;
        else locSettings.source[((cursorPosition / 10) % 10) - 1] = 11;
      }
    }
    //Ist CoilType gewählt?
    if (cursorPosition % 10 == 2) {
      if (!dir) {

        if (locSettings.coilType[((cursorPosition / 10) % 10) - 1] < 5) locSettings.coilType[((cursorPosition / 10) % 10) - 1]++;
        else locSettings.coilType[((cursorPosition / 10) % 10) - 1] = 0;
      }
      else {
        if (locSettings.coilType[((cursorPosition / 10) % 10) - 1] > 0) locSettings.coilType[((cursorPosition / 10) % 10) - 1]--;
        else locSettings.coilType[((cursorPosition / 10) % 10) - 1] = 5;
      }
    }
    setSettings(locSettings); //Lokale Kopie wieder in den original Struct schreiben
  }

  //MENÜ
  //Cursor soll nur von oben bis unten laufen
  else if (currentScreen == 1) {
    if (!dir) {
      if (encpos < encmax) encpos++;
      else encpos = 0;
    }
    else {
      if (encpos > 0) encpos--;
      else encpos = encmax;
    }

  }

  // FILE LIST
  // Dateiliste kann gescrolled werden
  else if (currentScreen == 2) {
    //Drehung im Uhrzeigersinn
    if (!dir) {
      //Oberhalb des unteren Displayendes
      if (encpos < encmax && encpos < (getNumberOfLoadedFiles() - 1)) encpos++;
      //In der letzten Zeile
      else {
        //Ist die Liste noch nicht zu Ende weiterscrollen
        if (getNumberOfLoadedFiles() > (scrollpos + encpos + 1)) scrollpos++;
        //Ist die Liste zu Ende wieder von vorne anfangen
        else {
          encpos = 0;
          scrollpos = 0;
        }
      }
    }
    else {
      //Drehung im Gegenuhrzeigersinn
      if (encpos > 0) encpos--;
      //In der ersten Zeile
      else {
        //Ist die Liste noch nicht zu Ende weiterscrollen
        if (scrollpos > 0) scrollpos--;
        //Ist die Liste zu Ende wieder von vorne anfangen
        else {
          //Sind weniger gleich 6 Elemente in der Liste?
          if (getNumberOfLoadedFiles() <= 6) {
            scrollpos = 0;
            encpos = getNumberOfLoadedFiles() - 1 ;
          }
          //Sind mehr  als 6 Elemente in der Liste?
          else {
            scrollpos = getNumberOfLoadedFiles() - 6;
            encpos = encmax;
          }
        }
      }
    }
  }
  //Coil Setup
  //Cursor soll nur von oben bis unten laufen
  else if (currentScreen == 3) {
    if (!dir) {
      if (encpos < encmax) encpos++;
      else encpos = 0;
    }
    else {
      if (encpos > 0) encpos--;
      else encpos = encmax;
    }
  }


  //CoilTypeSetup Untermenü
  else if (currentScreen == 4) {
    if (!dir) {
      if (dutyCycle < 49.9) dutyCycle += 0.1;
      else dutyCycle = 0;
    }
    else {
      if (dutyCycle > 0) dutyCycle -= 0.1;
      else dutyCycle = 50;
    }
  }

  //Utilitys
  //Cursor soll nur von oben bis unten laufen
  else if (currentScreen == 6) {

    if (!dir) {
      if (encpos < encmax) encpos++;
      else encpos = 0;
    }
    else {
      if (encpos > 0) encpos--;
      else encpos = encmax;
    }

  }

  refreshScreen();
}


//Wird bei erkanntem Tastendruck aufgerufen
void onButtonClicked(uint8_t pin) {

  if (getPlayingState()) { //Wenn gerade die Wiedergabe läuft
    pauseFile(); //Widergabe beim Druck der Menü Taste pausieren
    pin = 0; //Diese Eingabe ignorieren
  }
  //Menü/Home Taste
  if (pin == 5) {

    cursorPosition = 0;
    cursorPosition1 = 0;
    cursorPositionOld = 255;
    encpos = 0;

    if (currentScreen == 0) currentScreen = 1;
    else currentScreen = 0;
  }

  //Abhängig vom aktuellen Bild sollen die Knöpfe unterschiedlich ausgewertet werden
  switch (currentScreen) {

    //Bei unspezifiziertem Bildschirm soll jede Taste zum Homescreen führen.
    default:
      cursorPosition = 0;
      cursorPosition1 = 0;
      cursorPositionOld = 255;
      encpos = 0;
      currentScreen = 0;
      stor = 0;

      break;
    //GRUNDBILD
    case 0:
      //Play Taste. Wenn im Moment Pause aktiv ist und eine Datei gewählt ist soll die Wiedergabe beginnen.
      if (pin == 6 && !getPlayingState() && (getFileList(getCurrentFile()) != 0)) playFile();
      //Wenn eine Taste gefrückt wird soll die Wiedergabe pausiert werden.

      //Auswahl der Felder
      //cursorPosition: Zehner sind die Spalte, einer die Zeile, von oben gelesen.
      //out1 Taste
      if (pin == 1) {
        //Ist der Cursor bereits in der Spalte die mit dem Taster gedrückt wurde?
        if (cursorPosition / 10 == 1 ) {
          //Ist der Cursor schon am höchsten Wert der Spalte angekommen?
          //Wenn nein, springe weiter
          if (cursorPosition < 12) cursorPosition++;
          //Wenn ja, geh an den Anfang zurück.
          else cursorPosition = 11;
        }
        else cursorPosition = 11;
      }
      //out2 Taste
      if (pin == 2) {
        //Ist der Cursor bereits in der Spalte die mit dem Taster gedrückt wurde?
        if (cursorPosition / 10 == 2) {
          //Ist der Cursor schon am höchsten Wert der Spalte angekommen?
          //Wenn nein, springe weiter
          if (cursorPosition < 22) cursorPosition++;
          //Wenn ja, geh an den Anfang zurück.
          else cursorPosition = 21;
        }
        else cursorPosition = 21;
      }

      //out3 Taste
      if (pin == 3) {
        //Ist der Cursor bereits in der Spalte die mit dem Taster gedrückt wurde?
        if (cursorPosition / 10 == 3) {
          //Ist der Cursor schon am höchsten Wert der Spalte angekommen?
          //Wenn nein, springe weiter
          if (cursorPosition < 32) cursorPosition++;
          //Wenn ja, geh an den Anfang zurück.
          else cursorPosition = 31;
        }
        else cursorPosition = 31;
      }

      //out4 Taste
      if (pin == 4) {
        //Ist der Cursor bereits in der Spalte die mit dem Taster gedrückt wurde?
        if (cursorPosition / 10 == 4) {
          //Ist der Cursor schon am höchsten Wert der Spalte angekommen?
          //Wenn nein, springe weiter
          if (cursorPosition < 42) cursorPosition++;
          //Wenn ja, geh an den Anfang zurück.
          else cursorPosition = 41;
        }
        else cursorPosition = 41;
      }
      break;
    // MENÜ
    case 1:
      if ( pin == 6) {

        //Gerade "SaveSettings" gewählt?
        if (encpos == 0) {
          saveSettings();
        }
        //Gerade "Player File" gewählt?
        if (encpos == 1) {
          //SD ohne Fehler gelesen
          if(initializeSD()== -1){
            currentScreen = 2;
          }
//          switch (initializeSD()) {
//            case -1:
//              currentScreen = 2;
//              break;
//
//            //Keine Karte
//            case 1:
//              printNotificationScreen(2);
//              Serial.print("Fehler 1");
//              break;
//            //Karte Leer
//            case 2:
//              printNotificationScreen(3);
//              break;
//            //Anderer Fehler
//            default:
//              printNotificationScreen(0);
//              break;
//          }
          
        }
        //Gerade "Coil Setup" gewählt?
        if (encpos == 2) {
          currentScreen = 3;
        }
        //Gerade "Coil Test/Manual Mode" gewählt?
        if (encpos == 3) {
          currentScreen = 5;
        }
        //Gerade "Self Test" gewählt?
        if (encpos == 4) {
          currentScreen = 6;
        }
        //Gerade "Credits" gewählt?
        if (encpos == 5) {
          currentScreen = 7;
        }
        encpos = 0;
        //Im neuen Screen soll der Encoder wieder bei 0 stehen
      }
      break;

    //PLAYER FILE
    case 2:
      if (pin == 6 && getNumberOfLoadedFiles() > 0) {
        setCurrentFile(scrollpos + encpos);
        setFileSelected(); //Setzt ein Bit was dem Mediaplayer zeigt dass eine neue Datei ausgewählt wurde.
        currentScreen = 0;
      }
      break;

    //CoilSetup Menü
    case 3:
      if (pin == 6) {
        cursorPosition = encpos;
        currentScreen = 4; //Nach erster Auswahl ins Untermenü gehen
      }
      break;

    //CoilTypeSetup Untermenü
    case 4:
      //Zurück Taste
      if (pin == 1) {
        //Speichern des aktuellen Wertes
        int eeReadAddress = eeDCAddress + (cursorPosition * 512) + (4 * cursorPosition1 );
        EEPROM.put(eeReadAddress, stor);

        //Zurück gehen
        cursorPosition = 0;
        cursorPosition1 = 0;
        cursorPositionOld = 255;
        currentScreen = 3;

      }
      //Pfeil nach links
      if (pin == 2) {

        //Speichern des aktuellen Wertes
        int eeReadAddress = eeDCAddress + (cursorPosition * 512) + (4 * cursorPosition1 );
        EEPROM.put(eeReadAddress, stor);

        //Nach links gehen
        if (cursorPosition1 == 0) cursorPosition1 = 127;
        else cursorPosition1--;

      }
      //Pfeil nach rechts
      if (pin == 3) {
        //Speichern des aktuellen Wertes
        int eeReadAddress = eeDCAddress + (cursorPosition * 512) + (4 * cursorPosition1 );
        EEPROM.put(eeReadAddress, stor);

        //Nach rechts gehen
        if (cursorPosition1 == 127) cursorPosition1 = 0;
        else cursorPosition1++;

      }
      if (pin == 4) {
        //DutyCycle auf 50 % setzen
        dutyCycle = 25;
      }
      break;

    //UTILITIES
    case 6:
      //Save Taste
      if (pin == 1) {

        //Zurück gehen
        cursorPosition = 0;
        cursorPosition1 = 0;
        cursorPositionOld = 255;
        currentScreen = 0;
        //Speichern des aktuellen Wertes
        saveSettings();
      }
      //Eine andere Taste als die Save Taste gedrückt
      else {
        set locSettings = getSettings(); //Zieht eine lokale Kopie des Structs Settings

        //Autoplay
        if (encpos == 0) {

          //Jede relevante Taste soll bloß umschalten
          if (pin == 2 || pin == 3 || pin == 6) {
            locSettings.autoplay = !locSettings.autoplay;
          }
        }
        //MIDI AD Channel
        else if (encpos == 1) {

          if (pin == 3 || pin == 6) {
            if (locSettings.MIDIADChannel < 15) locSettings.MIDIADChannel++;
            else locSettings.MIDIADChannel = 3;
          }
          if (pin == 2 )
            if (locSettings.MIDIADChannel > 3) locSettings.MIDIADChannel--;
            else locSettings.MIDIADChannel = 15;
        }

        //Pitch Bend range
        else if (encpos == 2) {
          if (pin == 3 || pin == 6) {
            if (locSettings.pitchBendRange < 24) locSettings.pitchBendRange++;
            else locSettings.pitchBendRange = 0;
          }
          if (pin == 2 )
            if (locSettings.pitchBendRange > 0) locSettings.pitchBendRange--;
            else locSettings.pitchBendRange = 24;

        }
        //Serial Debugging
        else if (encpos == 3) {
          if (pin == 2 || pin == 3 || pin == 6) {
            locSettings.serialDebugging = !locSettings.serialDebugging;
          }
        }
        //Print File List
        else if (encpos == 4) {
          printFileList();
        }

        setSettings(locSettings); //Lokale Kopie wieder in den original Struct schreiben
      }
      break;
  }
  refreshScreen();
}


//Display Initialisierung
void initialiseDisplay(void) {

  u8g2.begin();

  u8g2.setFont(u8g2_font_6x10_tf);
  //Hintergrund transparent
  u8g2.setFontMode(1);
  //Schriftfarbe XOR
  u8g2.setDrawColor(2);
}

//Startbild anzeigen
void printStartScreen(void) {
  u8g2.clearBuffer(); //Full Buffer Mode: Buffer leeren
  u8g2.drawXBMP(0, 0, 256, 64, logo);
  u8g2.drawStr(97, 50, softwareVersion);

  u8g2.sendBuffer(); //Full Buffer Mode: senden

}

//Grundbild anzeigen
void printHomeScreen(bool locinputState[3]) {
  char out[5]; //Zwischenspeicher für Anzeigen auf dem Display

  int height = u8g2.getMaxCharHeight() + 3;
  int width = u8g2.getMaxCharWidth();

  u8g2.clearBuffer(); //Full Buffer Mode: Buffer leeren

  //out1
  //untere Schaltfläche
  u8g2.drawRFrame(xcol1, ybot, 29, height, 4);
  u8g2.drawStr(xcol1 + 3, 64, "out1");

  //CoilType Anzeige
  //Ist das Feld ausgewählt fülle das Feld aus
  if (cursorPosition == 12) u8g2.drawBox(xcol1, yrow1, 29, height);
  else u8g2.drawFrame(xcol1, yrow1, 29, height);

  u8g2.drawStr(xcol1 + 3, yrow1 + height - 3, "ct");
  intToString(getSettings().coilType[0]).toCharArray(out, 5); //Siehe ein paar Zeilen weiter unten
  u8g2.drawStr(width * 2 + 3 + xcol1, yrow1 + height - 3, out );


  //Source Anzeige
  if (cursorPosition == 11) u8g2.drawBox(xcol1, yrow2, 29, height);
  else u8g2.drawFrame(xcol1, yrow2, 29, height);

  sourceToString(getSettings().source[0]).toCharArray(out, 5); //Vor der Übergabe an drawstr muss der von der Methode "sourceToString" übergebene String in ein Char Array konvertiert werden, die Methode keinen String schluckt. Rückgabe von Arrays ist in c nicht möglich.
  u8g2.drawStr(3, yrow2 + height - 3, out);

  //out2
  //untere Schaltfläche
  u8g2.drawRFrame(xcol2, ybot, 29, height, 4);
  u8g2.drawStr(xcol2 + 3, 64, "out2");

  //CoilType Anzeige
  if (cursorPosition == 22) u8g2.drawBox(xcol2, yrow1, 29, height);
  else u8g2.drawFrame(xcol2, yrow1, 29, height);
  u8g2.drawStr(xcol2 + 3, yrow1 + height - 3, "ct");
  intToString(getSettings().coilType[1]).toCharArray(out, 5);
  u8g2.drawStr(width * 2 + 3 + xcol2, yrow1 + height - 3, out);

  //Source Anzeige
  if (cursorPosition == 21) u8g2.drawBox(xcol2, yrow2, 29, height);
  else u8g2.drawFrame(xcol2, yrow2, 29, height);
  sourceToString(getSettings().source[1]).toCharArray(out, 5);
  u8g2.drawStr(xcol2 + 3, yrow2 + height - 3, out);

  //out3
  //untere Schaltfläche
  u8g2.drawRFrame(xcol3, ybot, 29, height, 4);
  u8g2.drawStr(xcol3 + 3, 64, "out3");

  //CoilType Anzeige
  if (cursorPosition == 32) u8g2.drawBox(xcol3, yrow1, 29, height);
  else u8g2.drawFrame(xcol3, yrow1, 29, height);

  u8g2.drawStr(xcol3 + 3, yrow1 + height - 3, "ct");
  intToString(getSettings().coilType[2]).toCharArray(out, 5);
  u8g2.drawStr(width * 2 + 3 + xcol3, yrow1 + height - 3, out);

  //Source Anzeige
  if (cursorPosition == 31) u8g2.drawBox(xcol3, yrow2, 29, height);
  else u8g2.drawFrame(xcol3, yrow2, 29, height);
  sourceToString(getSettings().source[2]).toCharArray(out, 5);
  u8g2.drawStr(xcol3 + 3, yrow2 + height - 3, out);

  //out4
  //untere Schaltfläche
  u8g2.drawRFrame(xcol4, ybot, 29, height, 4);
  u8g2.drawStr(xcol4 + 3, 64, "out4");

  //CoilType Anzeige
  if (cursorPosition == 42) u8g2.drawBox(xcol4, yrow1, 29, height);
  else u8g2.drawFrame(xcol4, yrow1, 29, height);

  u8g2.drawStr(xcol4 + 3, yrow1 + height - 3, "ct");
  intToString(getSettings().coilType[3]).toCharArray(out, 5);
  u8g2.drawStr(width * 2 + 3 + xcol4, yrow1 + height - 3, out);

  //Source Anzeige
  if (cursorPosition == 41) u8g2.drawBox(xcol4, yrow2, 29, height);
  else u8g2.drawFrame(xcol4, yrow2, 29, height);
  sourceToString(getSettings().source[3]).toCharArray(out, 5);
  u8g2.drawStr(xcol4 + 3, yrow2 + height - 3, out);

  //menu
  u8g2.drawRFrame(xcol5, ybot, 29, height, 4);
  u8g2.drawStr(xcol5 + 3, ybot + 9, "menu");

  //Kopfzeile

  //MIDI IN
  u8g2.drawFrame(xtop1, ytop, width * 7 + 3, height);
  u8g2.drawStr(xtop1 + 2, ytop + 9, "MIDI in");
  if (locinputState[0]) u8g2.drawDisc(xtop1 + width * 7 + 8, 6, 3, U8G2_DRAW_ALL); //Wenn das jeweilige State Byte gesetzt ist soll der Kreis gefüllt werden.
  else u8g2.drawCircle(xtop1 + width * 7 + 8, 6, 3, U8G2_DRAW_ALL);  //Andernfalls eben nicht

  //Analog 1
  u8g2.drawFrame(xtop2, ytop, width * 7 + 3, height);
  u8g2.drawStr(xtop2 + 2, ytop + 9, "Analog1");
  if (locinputState[1])u8g2.drawDisc(xtop2 + width * 7 + 8, 6, 3, U8G2_DRAW_ALL);
  else u8g2.drawCircle(xtop2 + width * 7 + 8, 6, 3, U8G2_DRAW_ALL);
  //Analog 2

  u8g2.drawFrame(xtop3, ytop, width * 7 + 3, height);
  u8g2.drawStr(xtop3 + 2, ytop + 9, "Analog2");
  if (locinputState[2]) u8g2.drawDisc(xtop3 + width * 7 + 8, 6, 3, U8G2_DRAW_ALL);
  else u8g2.drawCircle(xtop3 + width * 7 + 8, 6, 3, U8G2_DRAW_ALL);

  //Aktuell gewählte Datei
  char charBuffer[13];
  getFileList(getCurrentFile()).toCharArray(charBuffer, 13);
  u8g2.drawStr(xtop4, ytop + 9, charBuffer);

  //Playbutton
  u8g2.setFont(u8g2_font_open_iconic_all_4x_t);

  if (getPlayingState()) u8g2.drawGlyph(220, 48, 0x00d2);
  else u8g2.drawGlyph(220, 48, 0x00d3);
  u8g2.setFont(u8g2_font_6x10_tf);

  u8g2.sendBuffer(); //Full Buffer Mode: senden
}

void printMenuScreen(void) {

  const int ypos[6] = {10, 20, 30, 40, 50, 60};

  encmax = 5; //Der Cursor soll Werte zwischen 0 und 5 annehmen

  u8g2.clearBuffer(); //Full Buffer Mode: Buffer leeren

  u8g2.drawStr(xcursor, ypos[encpos], ">"); //Cursor an entsprechender Position anzeigen

  u8g2.drawStr(xtext, ypos[0], "Save Settings");
  u8g2.drawStr(xtext, ypos[1], "Player File");
  u8g2.drawStr(xtext, ypos[2], "Coil Setup");
  u8g2.drawStr(xtext, ypos[3], "Coil Test/Manual Mode");
  u8g2.drawStr(xtext, ypos[4], "Utilities");
  u8g2.drawStr(xtext, ypos[5], "Credits");

  //home button
  u8g2.drawRFrame(224, 55, 29, 13, 4);
  u8g2.drawStr(227, 64, "home");

  u8g2.sendBuffer(); //Full Buffer Mode: senden
}

void printNotificationScreen(byte error) {
  currentScreen = 255;
  stor = error;
  u8g2.clearBuffer(); //Full Buffer Mode: Buffer leeren

  switch (error) {
    default:
      u8g2.drawStr(0, 20, "unknown error");
      break;
    case 1:
      u8g2.drawStr(0, 20, "Saving successfull");
      break;
    case 2:
      u8g2.drawStr(0, 20, "No SD-Card");
      break;
    case 3:
      u8g2.drawStr(0, 20, "No Files on Card");
      break;
  }

  u8g2.drawStr(0, 35, "Press any key to continue");

  //home button
  u8g2.drawRFrame(224, 55, 29, 13, 4);
  u8g2.drawStr(227, 64, "home");

  u8g2.sendBuffer(); //Full Buffer Mode: senden
}

void printPlayerFileScreen(void) {
  const int ypos[6] = {10, 20, 30, 40, 50, 60};

  u8g2.clearBuffer(); //Full Buffer Mode: Buffer leeren
  char charBuffer[32];
  u8g2.drawStr(xcursor, ypos[encpos], ">"); //Cursor an entsprechender Position anzeigen
  //Anzeige der Anzahl der Dateien und der Nummer der ausgewählten Datei
  u8g2.setCursor(227, 10);
  u8g2.print(scrollpos + encpos + 1);
  u8g2.print("/");
  u8g2.print(getNumberOfLoadedFiles());

  //Ausgabe der aktuell anzuzeigenden Dateien auf dem Display
  for (int i = 0; i < 6; i++)
  {
    getFileList(i + scrollpos).toCharArray(charBuffer, 32);
    u8g2.drawStr(xtext, ypos[i], charBuffer);

  }

  //home button
  u8g2.drawRFrame(224, 55, 29, 13, 4);
  u8g2.drawStr(227, 64, "home");

  u8g2.sendBuffer(); //Full Buffer Mode: senden
}

void printCoilSetupScreen(void) {
  u8g2.clearBuffer(); //Full Buffer Mode: Buffer leeren


  const int ypos[6] = {10, 20, 30, 40, 50, 60};

  encmax = 5; //Der Cursor soll Werte zwischen 0 und 5 annehmen

  u8g2.clearBuffer(); //Full Buffer Mode: Buffer leeren

  u8g2.drawStr(xcursor, ypos[encpos], ">"); //Cursor an entsprechender Position anzeigen

  u8g2.drawStr(xtext, ypos[0], "Coil Type 0 (default)");
  u8g2.drawStr(xtext, ypos[1], "Coil Type 1");
  u8g2.drawStr(xtext, ypos[2], "Coil Type 2");
  u8g2.drawStr(xtext, ypos[3], "Coil Type 3");
  u8g2.drawStr(xtext, ypos[4], "Coil Type 4");
  u8g2.drawStr(xtext, ypos[5], "Coil Type 5");



  //home button
  u8g2.drawRFrame(224, 55, 29, 13, 4);
  u8g2.drawStr(227, 64, "home");

  u8g2.sendBuffer(); //Full Buffer Mode: senden
}

void printCoilSetupScreen2(void) {
  int height = u8g2.getMaxCharHeight() + 3;
  int width = u8g2.getMaxCharWidth();
  int eeReadAddress = eeDCAddress + (cursorPosition * 512) + (4 * cursorPosition1 );
  if (cursorPositionOld != cursorPosition1) {

    EEPROM.get(eeReadAddress, stor);
    dutyCycle = stor / 83886.08;
    cursorPositionOld = cursorPosition1;
  }

  float frequency = getFrequency(cursorPosition1) / 174.76;
  stor = dutyCycle * 83886.08;

  //DEBUG
  Serial.print(eeReadAddress);
  Serial.print('\t');
  Serial.println(stor, HEX);

  u8g2.clearBuffer(); //Full Buffer Mode: Buffer leeren

  //Titelzeile
  u8g2.drawStr(xtext, 10, "Coil Type ");
  u8g2.setCursor(xtext + width * 10, 10);
  u8g2.print(cursorPosition);

  //Erste Zeile

  u8g2.setCursor(xtext, 30);
  u8g2.print("MIDI Nr:");
  u8g2.print(cursorPosition1);
  u8g2.print(" (");
  u8g2.print(frequency);
  u8g2.print("Hz)");

  u8g2.setCursor(xtext + width * 25, 30);
  u8g2.print("Duty Cycle:");
  u8g2.print(dutyCycle, 1);
  u8g2.print("%");


  //untere Schaltfläche
  u8g2.drawRFrame(xcol1, ybot, 29, height, 4);
  u8g2.drawStr(xcol1 + 3, 64, "back");
  u8g2.drawRFrame(xcol2, ybot, 29, height, 4);
  u8g2.drawStr(xcol2 + 3, 64, " <- ");
  u8g2.drawRFrame(xcol3, ybot, 29, height, 4);
  u8g2.drawStr(xcol3 + 3, 64, " -> ");
  u8g2.drawRFrame(xcol4, ybot, 29, height, 4);
  u8g2.drawStr(xcol4 + 3, 64, "25% ");

  //cursorPosition ist hier der zu editierende CoilType, cursorPosition1 der ausgewählte Wert dieses CoilTypes



  //home button
  u8g2.drawRFrame(224, 55, 29, 13, 4);
  u8g2.drawStr(227, 64, "home");

  u8g2.sendBuffer(); //Full Buffer Mode: senden
}

void printCoilTestScreen(void) {
  u8g2.clearBuffer(); //Full Buffer Mode: Buffer leeren



  //home button
  u8g2.drawRFrame(224, 55, 29, 13, 4);
  u8g2.drawStr(227, 64, "home");

  u8g2.sendBuffer(); //Full Buffer Mode: senden
}

void printUtilityScreen(void) {
  int height = u8g2.getMaxCharHeight() + 3;
  int width = u8g2.getMaxCharWidth();
  const int ypos[6] = {10, 20, 30, 40, 50, 60};

  encmax = 4; //Der Cursor soll Werte zwischen 0 und 4 annehmen

  u8g2.clearBuffer(); //Full Buffer Mode: Buffer leeren
  //Menüelemente
  u8g2.drawStr(xcursor, ypos[encpos], ">"); //Cursor an entsprechender Position anzeigen

  u8g2.drawStr(xtext, ypos[0], "Autoplay");
  u8g2.drawStr(xtext, ypos[1], "MIDI AD Channel");
  u8g2.drawStr(xtext, ypos[2], "Pitch Bend Range");
  u8g2.drawStr(xtext, ypos[3], "Serial Debugging");
  u8g2.drawStr(xtext, ypos[4], "Print File List");

  //Werte
  if (getSettings().autoplay) u8g2.drawStr(xcol4, ypos[0], "ON");
  else u8g2.drawStr(xcol4, ypos[0], "OFF");

  u8g2.setCursor(xcol4, ypos[1]);
  u8g2.print(getSettings().MIDIADChannel + 1);

  u8g2.setCursor(xcol4, ypos[2]);
  u8g2.print(getSettings().pitchBendRange);

  if (getSettings().serialDebugging) u8g2.drawStr(xcol4, ypos[3], "ON");
  else u8g2.drawStr(xcol4, ypos[3], "OFF");

  //Untere Schaltflächen
  u8g2.drawRFrame(xcol1, ybot, 29, height, 4);
  u8g2.drawStr(xcol1 + 3, 64, "save");
  u8g2.drawRFrame(xcol2, ybot, 29, height, 4);
  u8g2.drawStr(xcol2 + 12, 64, "-");
  u8g2.drawRFrame(xcol3, ybot, 29, height, 4);
  u8g2.drawStr(xcol3 + 12, 64, "+");
  u8g2.drawRFrame(xcol4, ybot, 29, height, 4);
  //home button
  u8g2.drawRFrame(224, 55, 29, 13, 4);
  u8g2.drawStr(227, 64, "home");

  u8g2.sendBuffer(); //Full Buffer Mode: senden
}

void printCreditsScreen(void) {
  u8g2.clearBuffer(); //Full Buffer Mode: Buffer leeren

  u8g2.drawStr(10, 10, "Controller for a Tesla Orchestra");
  u8g2.drawStr(10, 20, "Developed at FHWS Schweinfurt in 2019 by");
  u8g2.drawStr(10, 30, "Julius Witte");
  u8g2.drawStr(10, 40, "Michael Loose");
  u8g2.drawStr(10, 50, "Mark Philipp Richter");
  u8g2.drawStr(10, 60, "Hussein Mahmoud");


  //home button
  u8g2.drawRFrame(224, 55, 29, 13, 4);
  u8g2.drawStr(227, 64, "home");

  u8g2.sendBuffer(); //Full Buffer Mode: senden
}




//wandelt die Nummer des Eingangstyps in einen lesbaren String um
//Rückgabe eines Objektes vom Typ String ist relativ Speicherintensiv. Besser wäre es einen Puffer außerhalb der Methode zu erstellen und lediglich einen Zeiger darauf zurückzugeben.
//Das hat aber aus irgend einem Grund nicht funktioniert. Problem war dass der Puffer einfach nicht beschrieben wurde.
String sourceToString (int source) {
  String out;
  switch (source) {
    case 1:
      out = "MIDA";
      break;
    case 2:
      out = "MID1";
      break;
    case 3:
      out = "MID2";
      break;
    case 4:
      out = "MID3";
      break;
    case 5:
      out = "MID4";
      break;
    case 6:
      out = "Ply1";
      break;
    case 7:
      out = "Ply2";
      break;
    case 8:
      out = "Ply3";
      break;
    case 9:
      out = "Ply4";
      break;
    case 10:
      out = "ANI1";
      break;
    case 11:
      out = "ANI2";
      break;
    default:
      out = "none";
      break;

      return out;
  }
}

//Konvertiert eine Int Zahl in einen zweistelligen String
String intToString(int zahl) {
  if (zahl >= 0 && zahl < 10) {
    char buf1[3];
    buf1[0] = '0';
    buf1[1] = '0' + zahl;
    buf1[2] = '\0';
    return buf1;
  }
  else if (zahl >= 10 && zahl < 100) {
    char buf2[3];
    sprintf (buf2, "%1i", zahl);
    return buf2;
  }
  return "NULL";
}
