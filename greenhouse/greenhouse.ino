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

unsigned int  uiWaterVolume = 0;
unsigned char WaterflowFlag = 0;
unsigned int  WaterflowRate = 0;  // L/Hour
unsigned int  NbTopsFan     = 0;  // count the edges

unsigned char EncoderFlag = 0;
unsigned long StartTime   = 0;
unsigned char ButtonFlag  = 0;
signed   char LCDPage     = 4;
unsigned char SwitchtoWateringFlag = 0;
unsigned char SwitchtoWarningFlag  = 0;
unsigned char SwitchtoStandbyFlag  = 0;
unsigned char UpdateDataFlag  = 0;
unsigned char ButtonIndex = 0;
unsigned char EEPROMAddress = 0;
float Volume     = 0;
unsigned long counter = 0;


float DHTHumidity    = 0;
float DHTTemperature = 0;
float MoisHumidity   = 100;
float UVIndex        = 0;
char buffer[30];

// counters for test
int clicks = 0;
int encodercntr = 0;

void myCallback(int newValue) {
  encodercntr++;
}

GroveEncoder myEncoder(EncoderPin1, &myCallback);
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
  //attachInterrupt(0,ButtonClick,FALLING);
  pinMode(EncoderPin1, INPUT);
  pinMode(EncoderPin2, INPUT);
  //attachInterrupt(1,EncoderRotate,RISING);
  pinMode(WaterflowPin, INPUT);
  pinMode(RelayPin, OUTPUT);

  /* Init UV */
  while (!SI1145.Begin()) {
    delay(1000);
  }
  //StartTime = millis();
}

void loop() {
  myPump.off();

  // Read from the Moisturesensor
  MoisHumidity   = analogRead(MoisturePin) / 7;
  // show MoisHuidity on screen
  displayMoisture();

  //read from waterflow
  WaterflowRate = waterFlowSensor.measure_flow_rate();
  // show waterflowrate on screen
  displayWaterFlowRate();

  int buttonState = digitalRead(ButtonPin);

  if (buttonState == HIGH) {
    SeeedOled.setTextXY(2, 0);
    sprintf(buffer, "Clicks: %d", ++clicks);
    SeeedOled.putString(buffer);
  }

  SeeedOled.setTextXY(3, 0); //Set the cursor to Xth Page, Yth Column
  sprintf(buffer, "Encoder: %d", encodercntr);
  SeeedOled.putString(buffer);

}

void displayMoisture()
{
  sprintf(buffer, "Moisture: ", "%d.%d%%", (int)(MoisHumidity), (int)((int)(MoisHumidity * 100) % 100));
  SeeedOled.setTextXY(1, 0);         //Set the cursor to Xth Page, Yth Column
  SeeedOled.putString(buffer);
}

void displayWaterFlowRate(){
  sprintf(buffer,"%2d L/H",WaterflowRate);
            SeeedOled.setTextXY(5,10);
            SeeedOled.putString(buffer);

            if ((int)((int)(Volume*100) %100) < 10 ) {
                sprintf(buffer,"%2d.0%d L",(int)(Volume),(int)((int)(Volume*100) %100));
            } else {
                sprintf(buffer,"%2d.%2d L",(int)(Volume),(int)((int)(Volume*100) %100));
            }
            SeeedOled.setTextXY(6,10);
            SeeedOled.putString(buffer); 
            }


