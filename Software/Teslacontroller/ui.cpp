//gui.cpp
//
//User Interface für den Teslacontroller
//Enthält alle Methoden für das Auslesen der Knöpfe und die Anzeigen auf dem Display
//
//Author: Michael Loose
//FHWS Fakultät Elektrotechnik
//
//Erstellung: 3/2019
//Stand: 19/03/2019
//////////////////////////////////////////////////////////////////////

bool inputState[3] = {0, 0, 0}; //Die Anzeigen ob ein Eingang aktiv ist. Die sollen nicht hier bleiben, sondern kommen später dahin wos mehr sinn macht.

#include "ui.hpp"

set settings; //Variable zum Speichern der Einstellungen erstellen

set getSettings(void) {
  return settings;
}

//Aktuell angezeigtes Bild
//0: Grundbild
//1: Menü
byte currentScreen = 0;

void refreshScreen(byte locCurrentScreen, bool locInputState[3]) {
  switch (locCurrentScreen) {
    case 0:
      printHomeScreen(locInputState);
      break;
    case 1:
      printMenuScreen();
      break;
  }
}

//Verwaltung des Encoders

int encPosition = 0;

void onEncoderChange(int newValue) {
  Serial.print("Encoder change ");
  Serial.println(newValue);
  encPosition = newValue;
  refreshScreen(currentScreen, inputState);
}

void onButtonClicked(uint8_t pin,  bool heldDown) {
  Serial.print("S");
  Serial.print(pin);
  Serial.println(heldDown ? " Held" : " Pressed");
  if (pin == 7 && heldDown == false) {
    if (currentScreen == 0) currentScreen = 1;
    else currentScreen = 0;
  }
  refreshScreen(currentScreen, inputState);
}
void loadSettings() {
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

}
//Initialisierung der Eingabeknöpfe
void initialiseButtons(void) {
  Wire.begin();
  switches.initialiseInterrupt(ioFrom8574(pcf8575adress, 2), false);

  setupRotaryEncoderWithInterrupt(0, 1, onEncoderChange);
  Wire.requestFrom(pcf8575adress, 1); //Erste Abfrage der I2C Schnittstelle damit der PCF8575 die Interrupts korrekt setzt.

  switches.changeEncoderPrecision(255, 0);
  switches.addSwitch(2, onButtonClicked);
  switches.addSwitch(3, onButtonClicked);
  switches.addSwitch(4, onButtonClicked);
  switches.addSwitch(5, onButtonClicked);
  switches.addSwitch(6, onButtonClicked);
  switches.addSwitch(7, onButtonClicked);
  Serial.println("PCF8575 erfolgreich initialisiert!");
}

//Definition des Displayanschlusses
U8G2_SSD1322_NHD_256X64_F_4W_SW_SPI u8g2(U8G2_R2, /* clock=*/ 52, /* data=*/ 51, /* cs=*/ 49, /* dc=*/ 48, /* reset=*/ 47);



//Display Initialisierung
void initialiseDisplay(void) {
  u8g2.begin();
  //u8g2.setFlipMode(1);
  u8g2.setFont(u8g2_font_6x10_tf);
  //u8g2.setFontRefHeightExtendedText();
  //u8g2.setDrawColor(1);
  //u8g2.setFontPosTop();
  //u8g2.setFontDirection(0);
}

//Startbild anzeigen
void printStartScreen(void) {
  u8g2.clearBuffer(); //Full Buffer Mode: Buffer leeren
  u8g2.drawXBMP(0, 0, 256, 64, logo);
  u8g2.sendBuffer(); //Full Buffer Mode: senden

}

//Grundbild anzeigen
void printHomeScreen(bool locinputState[3]) {
  set locsettings = settings;
  char out[5]; //Zwischenspeicher für Anzeigen auf dem Display

  int height = u8g2.getMaxCharHeight() + 3;
  int width = u8g2.getMaxCharWidth();
  const int xcol1 = 0;
  const int xcol2 = 55;
  const int xcol3 = 110;
  const int xcol4 = 165;
  const int xcol5 = 228;
  const int xtop1 = 0;
  const int xtop2 = 60;
  const int xtop3 = 120;

  const int ytop = 0;
  const int ybot = 55;
  const int yrow1 = 36;
  const int yrow2 = 24;

  u8g2.clearBuffer(); //Full Buffer Mode: Buffer leeren

  //out1
  //untere Schaltfläche
  u8g2.drawRFrame(xcol1, ybot, 27, height, 3);
  u8g2.drawStr(xcol1 + 2, 64, "out1");

  //CoilType Anzeige
  u8g2.drawFrame(xcol1, yrow1, 27, height);
  u8g2.drawStr(xcol1 + 2, yrow1 + height - 3, "ct");
  intToString(locsettings.coilType[0]).toCharArray(out, 5); //Siehe ein paar Zeilen weiter unten
  u8g2.drawStr(width * 2 + 2 + xcol1, yrow1 + height - 3, out );

  //Source Anzeige
  u8g2.drawFrame(xcol1, yrow2, 27, height);
  sourceToString(locsettings.source[0]).toCharArray(out, 5); //Vor der Übergabe an drawstr muss der von der Methode "sourceToString" übergebene String in ein Char Array konvertiert werden, die Methode keinen String schluckt. Rückgabe von Arrays ist in c nicht möglich.
  u8g2.drawStr(2, yrow2 + height - 3, out);

  //out2
  //untere Schaltfläche
  u8g2.drawRFrame(xcol2, ybot, 27, height, 3);
  u8g2.drawStr(xcol2 + 2, 64, "out2");

  //CoilType Anzeige
  u8g2.drawFrame(xcol2, yrow1, 27, height);
  u8g2.drawStr(xcol2 + 2, yrow1 + height - 3, "ct");
  intToString(locsettings.coilType[1]).toCharArray(out, 5);
  u8g2.drawStr(width * 2 + 2 + xcol2, yrow1 + height - 3, out);

  //Source Anzeige
  u8g2.drawFrame(xcol2, yrow2, 27, height);
  sourceToString(locsettings.source[1]).toCharArray(out, 5);
  u8g2.drawStr(xcol2 + 2, yrow2 + height - 3, out);

  //out3
  //untere Schaltfläche
  u8g2.drawRFrame(xcol3, ybot, 27, height, 3);
  u8g2.drawStr(xcol3 + 2, 64, "out3");

  //CoilType Anzeige
  u8g2.drawFrame(xcol3, yrow1, 27, height);
  u8g2.drawStr(xcol3 + 2, yrow1 + height - 3, "ct");
  intToString(locsettings.coilType[2]).toCharArray(out, 5);
  u8g2.drawStr(width * 2 + 2 + xcol3, yrow1 + height - 3, out);

  //Source Anzeige
  u8g2.drawFrame(xcol3, yrow2, 27, height);
  sourceToString(locsettings.source[2]).toCharArray(out, 5);
  u8g2.drawStr(xcol3 + 2, yrow2 + height - 3, out);

  //out4
  //untere Schaltfläche
  u8g2.drawRFrame(xcol4, ybot, 27, height, 3);
  u8g2.drawStr(xcol4 + 2, 64, "out4");

  //CoilType Anzeige
  u8g2.drawFrame(xcol4, yrow1, 27, height);
  u8g2.drawStr(xcol4 + 2, yrow1 + height - 3, "ct");
  intToString(locsettings.coilType[3]).toCharArray(out, 5);
  u8g2.drawStr(width * 2 + 2 + xcol4, yrow1 + height - 3, out);

  //Source Anzeige
  u8g2.drawFrame(xcol4, yrow2, 27, height);
  sourceToString(locsettings.source[3]).toCharArray(out, 5);
  u8g2.drawStr(xcol4 + 2, yrow2 + height - 3, out);

  //menu
  u8g2.drawRFrame(xcol5, ybot, 27, height, 3);
  u8g2.drawStr(xcol5 + 2, ybot + 9, "menu");

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


  u8g2.sendBuffer(); //Full Buffer Mode: senden
}

void printMenuScreen(void) {

  const int xtext = 10;
  const int xcursor = 0;
  const int ypos[6] = {10, 20, 30, 40, 50, 60};

  //switches.changeEncoderPrecision(5, 0); //Der Cursor soll Werte zwischen 0 und 5 annehmen

  u8g2.clearBuffer(); //Full Buffer Mode: Buffer leeren

  u8g2.drawStr(xcursor, ypos[encPosition], ">"); //Cursor an entsprechender Position anzeigen

  u8g2.drawStr(xtext, ypos[0], "Save Settings");
  u8g2.drawStr(xtext, ypos[1], "Player Setup");
  u8g2.drawStr(xtext, ypos[2], "Coil Setup");
  u8g2.drawStr(xtext, ypos[3], "Coil Test/Manual Mode");
  u8g2.drawStr(xtext, ypos[4], "Self Test");
  u8g2.drawStr(xtext, ypos[5], "Credits");

  //home button
  u8g2.drawRFrame(228, 55, 27, 13, 3);
  u8g2.drawStr(230, 64, "home");

  u8g2.sendBuffer(); //Full Buffer Mode: senden
}

void saveSettings(int loceeAddress, set locSettings) {
  EEPROM.put(loceeAddress , locSettings);
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
    case 12:
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
