//gui.cpp
//
//User Interface für den Teslacontroller
//Enthält alle Methoden für das Auslesen der Knöpfe und die Anzeigen auf dem Display
//
//Author: Michael Loose
//FHWS Fakultät Elektrotechnik
//
//Erstellung: 3/2019
//Stand: 14/03/2019
//////////////////////////////////////////////////////////////////////

#include "ui.hpp"
#define pcf8575adress 0x20


//Bits zum Speichern der aktuellen Encoderdrehrichtung
bool encplus = true;
bool encminus = true;
volatile bool buttonPressed;

bool getButtonPressed(void){
  return buttonPressed;
}

//Initialisierung der Eingabeknöpfe
void initialiseButtons(void) {
  buttonPressed = 0; // Bit was durch die Interrupt Methode auf 1 gesetzt wird, sobald eine Benutzereingabe erfolgt
  Wire.begin(); //Eröffnen der I2C Schnittstelle
  pinMode(2, INPUT_PULLUP); //Aktivieren des internen Pullups für den Interruptpin
  attachInterrupt(digitalPinToInterrupt(2), pcf8575ISR, FALLING); //Initialisierung der Interruptroutine
  Wire.requestFrom(pcf8575adress, 1); //Erste Abfrage der I2C Schnittstelle damit der PCF8575 die Interrupts korrekt setzt.
  Serial.println("PCF8575 erfolgreich initialisiert!");
}

//Interrupt Routine. Beim Erkennen einer Benutzereingabe soll lediglich ein Bit gesetzt werden, um die Routine nicht unnötig zu belasten.
//Es ist an dieser Stelle nicht möglich bereits die I2C Schnittstelle auszulesen, da dazu ebenfalls eine Interruptroutine verwendet wird. Der ATMega würde sich sonst aufhängen.
void pcf8575ISR(void) {
  buttonPressed = 1;
}

//Auslesen der I2C Schnitstelle und bestimmen der erfolgten Benutzereingabe
void readButtons(void) {
  Wire.requestFrom(pcf8575adress, 1);
  byte incomingByte = Wire.read();


  if (incomingByte == 0b00000011) {
    encplus = true;
    encminus = true;
  }
  if ((incomingByte == 0b00000001) && encminus) {
    Serial.println("Enc+");
    encplus = false;
  }
  if ((incomingByte == 0b00000010) && encplus ) {
    Serial.println("Enc-");
    encminus = false;
  }
  if (incomingByte == 0b00000111) Serial.println("Enc Click");
  if (incomingByte == 0b00001011) Serial.println("Button 1");
  if (incomingByte == 0b00010011) Serial.println("Button 2");
  if (incomingByte == 0b00100011) Serial.println("Button 3");
  if (incomingByte == 0b01000011) Serial.println("Button 4");
  if (incomingByte == 0b10000011) Serial.println("Home");
  buttonPressed = 0;
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
  //u8g2.firstPage();
  //do {  // Wird benötigt falls Page Buffer statt Full buffer verwendet wird
  u8g2.clearBuffer(); //Full Buffer Mode: Buffer leeren

  u8g2.drawBitmap(0, 0, 32, 64, logo);
  //u8g2.drawFrame(0, 0, u8g2.getDisplayWidth(), u8g2.getDisplayHeight() ); //Rahmen um das Bild zeichnen


  u8g2.sendBuffer(); //Full Buffer Mode: senden
  //} while ( u8g2.nextPage() ); // Wird benötigt falls Page Buffer statt Full buffer verwendet wird
}

//Grundbild anzeigen, als Funktion  der Einstellungen
void printHomeScreen(set locsettings, bool locinputState[3]) {
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


  //u8g2.firstPage();
  //do {  // Wird benötigt falls Page Buffer statt Full buffer verwendet wird
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
  //} while ( u8g2.nextPage() ); // Wird benötigt falls Page Buffer statt Full buffer verwendet wird
}

void printMenuScreen(int locMenuCursorPosition) {

  const int xtext = 10;
  const int xcursor = 0;
  const int ypos[6] = {10, 20, 30, 40, 50, 60};

  u8g2.clearBuffer(); //Full Buffer Mode: Buffer leeren

  u8g2.drawStr(xcursor, ypos[locMenuCursorPosition], ">"); //Cursor an entsprechender Position anzeigen

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
