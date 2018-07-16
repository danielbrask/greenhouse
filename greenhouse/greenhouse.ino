#include <Wire.h>
#include <SeeedOLED.h>
#include <EEPROM.h>
#include "Arduino.h"
#include <relay.h>
#include "DHT.h"
#include "SI114X.h"
#include "soil_moisture_sensor.h"
#include "GroveEncoder.h"
#include "water_flow_sensor.h"

const int DHTPIN = 0xA0;    // what pin we're connected to
const int MoisturePin = 0xA1;
const int ButtonPin = 0x2;
const uint8_t DHTTYPE = DHT11;
const int EncoderPin1 = 0x3;
const int EncoderPin2 = 0x4;
const int WaterflowPin = 0x5;
const int RelayPin = 0x6;

// counters for test
bool isPumpOn = false;
bool ButtonClicked = false;
int clicks = 0;
int encodercntr = 0;

//Variables for displaying information on screen
float value = 0;

const int programInt = 0; // remove this after defining the proper way of handling programs

void myCallback(int newValue) {
  encodercntr++;
}

struct Global{
  float MoisHumidity;
  float WaterflowRate;
  uint16_t  vis;
  uint16_t  ir;
  uint16_t  uv;
};

Relay myPump(RelayPin);
SoilMoistureSensor moistureSensor(MoisturePin);
SI114X SI1145 = SI114X();
DHT humiditySensor(DHTPIN, DHTTYPE);
WaterFlowSensor waterFlowSensor(WaterflowPin);

void setup() {
  Wire.begin();
  SeeedOled.init();  //initialze SEEED OLED display
  SeeedOled.clearDisplay();          //clear the screen and set start position to top left corner
  SeeedOled.setNormalDisplay();      //Set display to normal mode (i.e non-inverse mode)
  SeeedOled.setPageMode();           //Set addressing mode to Page Mode
  SeeedOled.setTextXY(0, 0);         //Set the cursor to Xth Page, Yth Column
  SeeedOled.putString("Smart Greenhouse!");

  /* Init DHT11 */
  Serial.begin(9600);
  Serial.println("Starting up...");
  humiditySensor.begin();

  /* Init Button */
  pinMode(ButtonPin, INPUT);
  attachInterrupt(0,ButtonClick,FALLING);

  //pinMode(EncoderPin1, INPUT);
  pinMode(EncoderPin2, INPUT);
  attachInterrupt(1,EncoderRotate,RISING);

  pinMode(WaterflowPin, INPUT);
  pinMode(RelayPin, OUTPUT);

  /* Init UV */
   while (!SI1145.Begin()) {
     delay(1000);
   }
}

Global global;

void loop() {

  global.MoisHumidity = humiditySensor.readHumidity();
  global.WaterflowRate = waterFlowSensor.measure_flow_rate();
  global.vis = SI1145.ReadVisible();
  global.ir = SI1145.ReadIR();
  global.uv = SI1145.ReadUV();

  //Code for running the different programs
  switch(encodercntr%2){
      case 0:
        overviewProgram();
        break;
      
      case 1:
        SeeedOled.clearDisplay();
        chilliProgram();
        break;
      
      case 2:
        SeeedOled.clearDisplay();
        carrotProgram();
        break;
      
      default:
        break;
    }
}

/**
 * Displays information on the screen
 * description: the variable one wishes to show 
 * value: the value of the variable shown
 * unit: the unit of measurement used for the value
 * xCoordinate: the x-coordinate where to write on the display
 * yCoordinate: the y-coordinate where to write on the display
 */
void displayText(const char* description, int value, int xCoordinate, int yCoordinate) {
  String str =  String(description) + String(value);
  SeeedOled.setTextXY(xCoordinate, yCoordinate);         //Set the cursor to Xth Page, Yth Column
  SeeedOled.putString(str.c_str());
}

void ButtonClick() {
  if(digitalRead(ButtonPin) == 0) {
    delay(10);
    if(digitalRead(ButtonPin) == 0) {
      ButtonClicked = true;
    }
  }
}

//Shows readings from all sensors and starts pump with button
void overviewProgram() {

if (ButtonClicked == true) {
    if(isPumpOn == false){
      myPump.on();
      isPumpOn = !isPumpOn;
    } else if(isPumpOn == true) {
      myPump.off();
      isPumpOn = !isPumpOn;
    }
    displayText("Clicks: ", clicks, 2, 0);
    clicks++;
    ButtonClicked = false;
  }

  displayText("Moisture: ",(int)global.MoisHumidity ,1 ,0 );
  displayText("Encoder: ", encodercntr, 3, 0);
  displayText("Waterflow: ", global.WaterflowRate, 4, 0);
  displayText("Sun: ", (int)global.vis, 5, 0);
  displayText("IR: ", (int)global.ir, 6, 0);
  displayText("UV: ", (int)global.uv, 7, 0);
  
  }

//Method to be used by the loop for handling chilli plants
void chilliProgram()
{

  if(global.MoisHumidity < 50 && !isPumpOn){
    //myPump.on();
    //isPumpOn = !isPumpOn;
    delay(3000);
    myPump.off();
  }
  }
  
//Method to be used by the loop for handling chilli plants
void carrotProgram() {
}

void refreshScreen() {

}

int encoderPos = 0;
int encoderPinLast = LOW;
int n = LOW;

 void EncoderRotate() {
     n = digitalRead(EncoderPin1);
  if ( encoderPinLast == LOW && 
         n == HIGH ) {
    if ( digitalRead(EncoderPin2) == LOW ) {
      encodercntr--;
    } else {
      encodercntr++;
    }
  }
 }







