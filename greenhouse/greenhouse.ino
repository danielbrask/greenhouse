#include <Encoder.h>

#include <Wire.h>
#include <SeeedOLED.h>
#include <EEPROM.h>
#include "Arduino.h"
#include <relay.h>
#include "DHT.h"
#include "SI114X.h"
#include "soil_moisture_sensor.h"
#include "water_flow_sensor.h"
#include "Encoder.h"

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


//GroveEncoder myEncoder(EncoderPin1, &myCallback);
Encoder myEncoder(EncoderPin1, 0);
Relay myPump(RelayPin);
SoilMoistureSensor moistureSensor(MoisturePin);
SI114X SI1145 = SI114X();
DHT humiditySensor(DHTPIN, DHTTYPE);
WaterFlowSensor waterFlowSensor(WaterflowPin);
//GroveButton myButton(ButtonPin)

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
  //StartTime = millis();
}

void loop() {
  auto MoisHumidity = humiditySensor.readHumidity();
  auto WaterflowRate = waterFlowSensor.measure_flow_rate();
  auto _vis = SI1145.ReadVisible();
  auto _ir = SI1145.ReadIR();
  auto _uv = SI1145.ReadUV();

  displayText("Moisture: ",(int)(MoisHumidity) ,1 ,0 );
  displayText("Waterflow: ", WaterflowRate, 4, 0);
  displayText("Sun: ", (int)_vis, 5, 0);
  displayText("IR: ", (int)_ir, 6, 0);
  displayText("UV: ", (int)_uv, 7, 0);
  displayText("Clicks: ", 2, 0);
  displayText("Encoder: ", encodercntr, 3, 0);

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

void displayText(const char* description, int xCoordinate, int yCoordinate) {
  SeeedOled.setTextXY(xCoordinate, yCoordinate);         //Set the cursor to Xth Page, Yth Column
  SeeedOled.putString(description);
}

void ButtonClick() {

  if(digitalRead(ButtonPin) == 0) {
    delay(20);
    if(digitalRead(ButtonPin) == 0) {
      ButtonClicked = true;
    }
  }
}

void EncoderRotate() {
if (myEncoder.rotate_flag == 1)
  {
    if (myEncoder.direct == 1)
    {
      encodercntr++;
    } else {
    encodercntr--;
    }
    encoder.rotate_flag = 0;
  }
}