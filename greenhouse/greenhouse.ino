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

//Variables for displaying information on screen
String desription = "";
float value = 0;
float xCoordinate = 0;
float yCoordinate = 0;

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
  //MoisHumidity   = analogRead(MoisturePin) / 7;
  MoisHumidity = humiditySensor.readHumidity();
  // show MoisHuidity on screen
  //displayMoisture();
  displayInformation("Moisture: ",(int)(MoisHumidity) ,1 ,0 );

  //read from waterflow
  WaterflowRate = 0; //waterFlowSensor.measure_flow_rate();
  // show waterflowrate on screen
  displayWaterFlowRate();

  uint16_t _vis = SI1145.ReadVisible();
  uint16_t _ir = SI1145.ReadIR();
  uint16_t _uv = SI1145.ReadUV();

  SeeedOled.setTextXY(7, 0);
  //char* str = "Sunlight: " << _vis << "ir "<< _ir << "uv " _uv;
  //SeeedOled.putString(str);

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

void displayMoisture() {
  sprintf(buffer, "Moisture: ", "%d.%d%%", (int)(MoisHumidity), (int)((int)(MoisHumidity * 100) % 100));
  SeeedOled.setTextXY(1, 0);         //Set the cursor to Xth Page, Yth Column
  SeeedOled.putString(buffer);
}

void displayWaterFlowRate() {
  sprintf(buffer,"%2d L/H",WaterflowRate);
            SeeedOled.setTextXY(5,10);
            SeeedOled.putString(buffer);

            if ((int)((int)(Volume*100) %100) < 10 ) {
                sprintf(buffer,"%2d.0%d L",(int)(Volume),(int)((int)(Volume*100) %100));
            
            }
            SeeedOled.setTextXY(6,10);
<<<<<<< HEAD
            SeeedOled.putString(buffer); 
            }

/**
 * Displays information on the screen
 * description: the variable one wishes to show 
 * value: the value of the variable shown
 * unit: the unit of measurement used for the value
 * xCoordinate: the x-coordinate where to write on the display
 * yCoordinate: the y-coordinate where to write on the display
 */
void displayInformation(const char description, int value, int xCoordinate, int yCoordiate){
  sprintf(buffer, description, "%d.%d%%", (int)(value), (int)((int)(value * 100) % 100));
  SeeedOled.setTextXY(xCoordinate, yCoordinate);         //Set the cursor to Xth Page, Yth Column
  SeeedOled.putString(buffer);
  
}


=======
            SeeedOled.putString(buffer);
}
>>>>>>> 0a908809e1403c0b38d02b781b937eabe60ae82e
