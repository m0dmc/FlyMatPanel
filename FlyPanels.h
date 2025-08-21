/* Create test program for matrix fly cinema */

#include "LED_Built_In.h"    // Creates class function LED to toggle etc
class LED_Control LEDext;

#define SerialPort Serial
#define BAUDRATE 115200
#define VERSION_LINE1 "# Source [" __FILE__ "]\r\n"
#define VERSION_LINE2 "# Compiled " __DATE__ " at " __TIME__ " - dmc\r\n"

#include "SerialIO.h"
class SerialIO SerialIO;

#define NUM_ROWS 3 // Number of rows of chained INDIVIDUAL PANELS (ROW >= 1 for virtual array)
#define NUM_COLS 1 // Number of INDIVIDUAL PANELS

#include "MyMatrix.h";
class MyMatrix MyMatrix;

#include "TermComm.h"
class TermComm TermComm;

#include "SpriteTest.h"

void setup() {
  SerialIO.Init();
  MyMatrix.Init();
  LEDext.Init(18); //io18
}


void loop() {
  SerialIO.Version();
  TermComm.ShowHelp();
  SerialIO.SerialOut(">"); // show prompt and wait for command
  do{
    if(SerialIO.CheckSerialIn()){
      if(!TermComm.CheckCommand())SerialIO.SerialOut("Error - Type h for help\r\n");
      SerialIO.SerialOut(">"); // show prompt and wait for command
    }
  } while(true); // forever


  Sprite.BounceTest();
  
}
