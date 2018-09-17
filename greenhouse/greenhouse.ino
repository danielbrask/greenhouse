#include "Wire.h"
#include "SeeedOLED.h"
#include "EEPROM.h"
#include "Arduino.h"
#include "relay.h"
#include "DHT.h"
#include "SI114X.h"
#include "soil_moisture_sensor.h"
#include "GroveEncoder.h"
#include "water_flow_sensor.h"

const int DHTPIN = 0xA0;
const int MoisturePin = 0xA1;
const int ButtonPin = 0x2;
const uint8_t DHTTYPE = DHT11;
const int EncoderPin1 = 0x3;
const int EncoderPin2 = 0x4;
const int WaterflowPin = 0x5;
const int RelayPin = 0x6;

struct State{
  float MoisHumidity = 0;
  float WaterflowRate = 0;
  
  uint16_t  visibleLight = 0;
  uint16_t  irLight = 0;
  uint16_t  uvLight = 0;
  uint16_t clicks = 0;
  uint16_t encodercntr = 0;

  bool isPumpOn = false;
  bool ButtonClicked = false;
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

State state;

void loop() {

  state.MoisHumidity = humiditySensor.readHumidity();
  state.WaterflowRate = waterFlowSensor.measure_flow_rate();
  state.visibleLight = SI1145.ReadVisible();
  state.irLight = SI1145.ReadIR();
  state.uvLight = SI1145.ReadUV();

  //Code for running the different programs
  switch(state.encodercntr%2){
      case 0:
        overviewProgram();
        break;
      
      case 1:
        SeeedOled.clearDisplay();
        chilliProgram(state);
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
      state.ButtonClicked = true;
    }
  }
}

//Shows readings from all sensors and starts pump with button
void overviewProgram() {

if (state.ButtonClicked == true) {
    if((state.isPumpOn == false) && (state.MoisHumidity > 355)){
      myPump.on();
      state.isPumpOn = !state.isPumpOn;
    } else if((state.isPumpOn == true) && (state.MoisHumidity < 355)) {
      myPump.off();
      state.isPumpOn = !state.isPumpOn;
    }
    displayText("Clicks: ", state.clicks, 2, 0);
    state.clicks++;
    state.ButtonClicked = false;
  }

  displayText("Moisture: ",(int)state.MoisHumidity ,1 ,0 );
  displayText("Encoder: ", state.encodercntr, 3, 0);
  displayText("Waterflow: ", state.WaterflowRate, 4, 0);
  displayText("Sun: ", (int)state.visibleLight, 5, 0);
  displayText("IR: ", (int)state.irLight, 6, 0);
  displayText("UV: ", (int)state.uvLight, 7, 0);
  
  }

//Method to be used by the loop for handling chilli plants
void chilliProgram(State& obj)
{

  if(state.MoisHumidity < 50 && !state.isPumpOn){
    //myPump.on();
    //isPumpOn = !isPumpOn;
    delay(3000);
    myPump.off();
  }
  }
  
//Method to be used by the loop for handling chilli plants
void carrotProgram() {
}

int encoderPos = 0;
int encoderPinLast = LOW;
int n = LOW;

 void EncoderRotate() {
     n = digitalRead(EncoderPin1);
  if ( encoderPinLast == LOW && 
         n == HIGH ) {
    if ( digitalRead(EncoderPin2) == LOW ) {
      state.encodercntr--;
    } else {
      state.encodercntr++;
    }
  }
 }
