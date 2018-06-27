#include <Wire.h>
#include <SeeedOLED.h>
#include <EEPROM.h>
#include "DHT.h"
#include <TimerOne.h>
#include "Arduino.h"
#include "SI114X.h"

void setup() {
  // put your setup code here, to run once:

  Wire.begin();
  SeeedOled.init();  //initialze SEEED OLED display

  SeeedOled.clearDisplay();          //clear the screen and set start position to top left corner
  SeeedOled.setNormalDisplay();      //Set display to normal mode (i.e non-inverse mode)
  SeeedOled.setPageMode();           //Set addressing mode to Page Mode
  SeeedOled.setTextXY(0,0);          //Set the cursor to Xth Page, Yth Column  
  SeeedOled.putString("Smart Greenhouse!");

}

void loop() {
  // put your main code here, to run repeatedly:

}
