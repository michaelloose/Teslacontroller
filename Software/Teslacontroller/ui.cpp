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


//Speichern die aktuelle encoder Drehrichtung
bool encplus = true;
bool encminus = true;
byte encpos = 0;
byte encmax = 5;
byte scrollpos = 0;
//Aktuell ausgewähltes Feld
byte cursorPosition = 0;



//Wird ständig gepolled. Prüft ob das Bit für die Benutzereingabe gesetzt wurde.
void pollUserInput() {

  //Auslesen der Daten über i2c
  Wire.requestFrom(pcf8575adress, 1);
  byte incomingByte = Wire.read();

  //Encoder in Ruhestellung?
  if (incomingByte == 0b00000011) {
    encplus = true;
    encminus = true;
  }
  //Encoder im Uhrzeigersinn?
  if ((incomingByte == 0b00000001) && encminus) {
    encplus = false;
    onEncoderChange(true);
  }
  //Encoder im Gegenuhrzeigersinn?
  if ((incomingByte == 0b00000010) && encplus ) {
    encminus = false;
    onEncoderChange(false);
  }
  //Taste gedrückt?
  if (incomingByte == 0b00000111) onButtonClicked(6);
  if (incomingByte == 0b00001011) onButtonClicked(1);
  if (incomingByte == 0b00010011) onButtonClicked(2);
  if (incomingByte == 0b00100011) onButtonClicked(3);
  if (incomingByte == 0b01000011) onButtonClicked(4);
  if (incomingByte == 0b10000011) onButtonClicked(5);

}

//Aktuell angezeigtes Bild
//0: Grundbild
//1: Menü
//2: Player File Auswahl
//3: Coil Setup
//4: Coil Test/Manual Mode
//5: Self Test
//6: Credits

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
      printCoilTestScreen();
      break;
    case 5:
      printSelfTestScreen();
      break;
    case 6:
      printCreditsScreen();
      break;
  }
}

//Verwaltung des Encoders



//Wird bei erkanntem Drehen des Encoders aufgerufen
void onEncoderChange(bool direction) {
  Serial.print("Encoder change ");
  // Ist gerade die Dateiauswahl aktiv?
  // Dateiliste kann gescrolled werden
  if (currentScreen == 2) {
    //Drehung im Uhrzeigersinn
    if (direction) {
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
  else {
    if (direction) {
      if (encpos < encmax) encpos++;
      else encpos = 0;
    }
    else {
      if (encpos > 0) encpos--;
      else encpos = encmax;
    }
  }
  Serial.println(encpos);

  //Wird gerade das Menü angezeigt?
  if (currentScreen == 0) {
    //Ist gerade Source ausgewählt?
    if (cursorPosition % 10 == 1) {
      //Schreibe den aktuellen Wert in das korrekte Feld.
      //Die Operation nimmt den Zehner von cursorPosition-1, also die entsprechende Ausgangsnummer

      if (direction) {
        if (getSettings().source[((cursorPosition / 10) % 10) - 1] < 11) setSettings.source[((cursorPosition / 10) % 10) - 1]++;
        else getSettings().source[((cursorPosition / 10) % 10) - 1] = 0;
      }
      else {
        if (getSettings().source[((cursorPosition / 10) % 10) - 1] > 0) setSettings.source[((cursorPosition / 10) % 10) - 1]--;
        else setSettings.source[((cursorPosition / 10) % 10) - 1] = 11;
      }
    }
    //Ist CoilType gewählt?
    if (cursorPosition % 10 == 2) {
      if (direction) {

        if (getSettings().coilType[((cursorPosition / 10) % 10) - 1] < 20) setSettings.coilType[((cursorPosition / 10) % 10) - 1]++;
        else setSettings.coilType[((cursorPosition / 10) % 10) - 1] = 0;
      }
      else {
        if (getSettings().coilType[((cursorPosition / 10) % 10) - 1] > 0) setSettings.coilType[((cursorPosition / 10) % 10) - 1]--;
        else setSettings.coilType[((cursorPosition / 10) % 10) - 1] = 20;
      }
    }

  }
  refreshScreen();
}
//Wird bei erkanntem Tastendruck aufgerufen
void onButtonClicked(uint8_t pin) {

  Serial.println(getCurrentFile());
  //Ausgabe fürs Debugging
  Serial.print("S");
  Serial.print(pin);
  //Menü/Home Taste
  if (pin == 5) {
    cursorPosition = 0;
    encpos = 0;

    if (currentScreen == 0) currentScreen = 1;
    else currentScreen = 0;
  }


  //Grundbild
  if (currentScreen == 0) {


    //Play Taste. Wenn im Moment Pause aktiv ist und eine Datei gewählt ist soll die Wiedergabe beginnen.
    if (pin == 6 && !getPlayingState() && (getFileList(getCurrentFile()) != 0)) playFile();
    //Wenn eine Taste gefrückt wird soll die Wiedergabe pausiert werden.
    else pauseFile();
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
  }
  // MENÜ
  if (currentScreen == 1) {
    //Encoder gedrückt
    if (pin == 6) {

      //Gerade "SaveSettings" gewählt?
      if (encpos == 0) {
        saveSettings();
        currentScreen = 0;
      }
      //Gerade "Player File" gewählt?
      if (encpos == 1) {
        encpos = 0;
        //SD ohne Fehler gelesen
        if (initializeSD() == -1) {
          printPlayerFileScreen();
          currentScreen = 2;
        }
        //SD Lesefehler
        else {
          //Hier soll noch einn FEHLER POPUP eingeblendet werden                                                                                      TBD
          currentScreen = 0;
        }
      }
      //Gerade "Coil Setup" gewählt?
      if (encpos == 2) {
        currentScreen = 3;
      }
      //Gerade "Coil Test/Manual Mode" gewählt?
      if (encpos == 3) {
        currentScreen = 4;
      }
      //Gerade "Self Test" gewählt?
      if (encpos == 4) {
        currentScreen = 5;
      }
      //Gerade "Credits" gewählt?
      if (encpos == 5) {
        currentScreen = 6;
      }



    }
  }
  //Bei der Dateiauswahl die aktuelle Datei übernehmen
  if (currentScreen == 2 && pin == 6 && getNumberOfLoadedFiles() > 0) {
    setCurrentFile(scrollpos + encpos);
    setFileSelected(); //Setzt ein Bit was dem Mediaplayer zeigt dass eine neue Datei ausgewählt wurde.
    //currentScreen = 0;
  }

  refreshScreen();
}



//Definition des Displayanschlusses
U8G2_SSD1322_NHD_256X64_F_4W_SW_SPI u8g2(U8G2_R2, /* clock=*/ 52, /* data=*/ 51, /* cs=*/ 49, /* dc=*/ 48, /* reset=*/ 47);



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
  u8g2.sendBuffer(); //Full Buffer Mode: senden

}

//Grundbild anzeigen
void printHomeScreen(bool locinputState[3]) {
  char out[5]; //Zwischenspeicher für Anzeigen auf dem Display

  int height = u8g2.getMaxCharHeight() + 3;
  int width = u8g2.getMaxCharWidth();
  const byte xcol1 = 0;
  const byte xcol2 = 54;
  const byte xcol3 = 108;
  const byte xcol4 = 162;
  const byte xcol5 = 224;
  const byte xtop1 = 0;
  const byte xtop2 = 60;
  const byte xtop3 = 120;
  const byte xtop4 = 180;

  const byte ytop = 0;
  const byte ybot = 55;
  const byte yrow1 = 36;
  const byte yrow2 = 23;

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
  char charBuffer[12];
  getFileList(getCurrentFile()).toCharArray(charBuffer, 12);
  u8g2.drawStr(xtop4, ytop + 9, charBuffer);

  //Playbutton
  u8g2.setFont(u8g2_font_open_iconic_all_4x_t);

  if (getPlayingState()) u8g2.drawGlyph(220, 48, 0x00d2);
  else u8g2.drawGlyph(220, 48, 0x00d3);
  u8g2.setFont(u8g2_font_6x10_tf);

  u8g2.sendBuffer(); //Full Buffer Mode: senden
}

void printMenuScreen(void) {

  const int xtext = 10;
  const int xcursor = 0;
  const int ypos[6] = {10, 20, 30, 40, 50, 60};

  encmax = 5; //Der Cursor soll Werte zwischen 0 und 5 annehmen

  u8g2.clearBuffer(); //Full Buffer Mode: Buffer leeren

  u8g2.drawStr(xcursor, ypos[encpos], ">"); //Cursor an entsprechender Position anzeigen

  u8g2.drawStr(xtext, ypos[0], "Save Settings");
  u8g2.drawStr(xtext, ypos[1], "Player File");
  u8g2.drawStr(xtext, ypos[2], "Coil Setup");
  u8g2.drawStr(xtext, ypos[3], "Coil Test/Manual Mode");
  u8g2.drawStr(xtext, ypos[4], "Self Test");
  u8g2.drawStr(xtext, ypos[5], "Credits");

  //home button
  u8g2.drawRFrame(224, 55, 29, 13, 4);
  u8g2.drawStr(227, 64, "home");

  u8g2.sendBuffer(); //Full Buffer Mode: senden
}
void printPlayerFileScreen(void) {


  const int xtext = 10;
  const int xcursor = 0;
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
void printSelfTestScreen(void) {
  u8g2.clearBuffer(); //Full Buffer Mode: Buffer leeren



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
  else if (zahl >= 10 && zahl < 99) {
    char buf2[3];
    sprintf (buf2, "%1i", zahl);
    return buf2;
  }
  return "NULL";
}
