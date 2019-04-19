//Teslacontroller.ino
//
//Main File für den Teslacontroller
//
//
//Author: Mark Phillipp Richter
//FHWS Fakultät Elektrotechnik
//
//Erstellung: 4/2019
//Stand: 19/04/2019
//////////////////////////////////////////////////////////////////////

//SYNTAX AUSGABE: readMidi(byte0, byte1, byte2)

#include "mediaplayer.hpp"

String fileList[64];
byte currentFile = 0;

int initializeSD(void){

fileList[0] = "File 1";
fileList[1] = "File 2";
fileList[2] = "File 3";

return -1;
  
}

void playFile(void){
  
}

void pauseFile(void){
  
}






//Ab hier sind nur noch die Getter/Settermethoden
String getFileList(int k){
  return fileList[k];
}
byte getCurrentFile(void){
return currentFile; 
}
void setCurrentFile(byte i){
currentFile = i; 
}
