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

const int DHTPIN = 0xA0; //Temprature Humidity sensor
const int MoisturePin = 0xA1; //Analogue
const byte ButtonPin = 2; //Digital for On
const byte ButtonPinOff = 3; //Digital for Off
const uint8_t DHTTYPE = DHT11;
const int WaterflowPin = 0x5;
const byte RelayPin = 6; // Relay for light
const byte RelayPin2 = 8; //Relay for pump


volatile byte ButtonClicked = 0;
volatile byte ButtonClickedOff = 0;

struct State {
  float MoisHumidity = 0;
  float WaterflowRate = 0;

  uint16_t  visibleLight = 0;
  uint16_t  irLight = 0;
  uint16_t  uvLight = 0;
  uint16_t clicks = 0;
  uint16_t encodercntr = 0;
  uint16_t moisture = 0;
  uint16_t  counts = 0;

  //bool isPumpOn = false;
  //volatile
  //  volatile bool ButtonClicked = false;
  //  volatile bool ButtonClickedOff = false;
};

Relay myLight(RelayPin); // For light
Relay myPump(RelayPin2); // For pump
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



  //digitalPinToInterrupt(buttonPin2)
  /* Init Button */
  pinMode(ButtonPin, INPUT); //Set this pin to be input (pin 2 as initialized before)
  attachInterrupt(0, ButtonClick, CHANGE);

  pinMode(ButtonPinOff, INPUT); //Set this pin to be input (pin 2 as initialized before)
  attachInterrupt(1, ButtonClickOff, CHANGE);


  pinMode(WaterflowPin, INPUT);
  pinMode(RelayPin, OUTPUT); // Pump
  pinMode(RelayPin2, OUTPUT); //Light

  /* Init UV */
  while (!SI1145.Begin()) {
    delay(1000);
  }
}

State state;

void loop() {

  state.MoisHumidity = moistureSensor.read_humidity(); // Reading values from Humidity sensor
  state.WaterflowRate = waterFlowSensor.measure_flow_rate();
  state.visibleLight = SI1145.ReadVisible();
  state.irLight = SI1145.ReadIR();
  state.uvLight = SI1145.ReadUV(); // Read from UV sensor
  //state.moisture = moistureSensor.read_humidity();
  //digitalPinToInterrupt(3)


  overviewProgram();

  /*
    //Code for running the different programs
    switch(state.encodercntr%2){
        case 0:

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
      } */
}

/**
   Displays information on the screen
   description: the variable one wishes to show
   value: the value of the variable shown
   unit: the unit of measurement used for the value
   xCoordinate: the x-coordinate where to write on the display
   yCoordinate: the y-coordinate where to write on the display
*/
void displayText(const char* description, int value, int xCoordinate, int yCoordinate) {
  String str =  String(description) + String(value);
  SeeedOled.setTextXY(xCoordinate, yCoordinate);         //Set the cursor to Xth Page, Yth Column
  SeeedOled.putString(str.c_str());
}



void ButtonClick() {
  if (digitalRead(ButtonPin) == 0) {
    delay(50);
    if (digitalRead(ButtonPin) == 0) {
      delay(50);
      ButtonClicked = 1;
    }
  }
}


void ButtonClickOff() {
  if (digitalRead(ButtonPinOff) == 0) {
    delay(70);
    if (digitalRead(ButtonPinOff) == 0) {
      delay(50);
      ButtonClickedOff = 1;
    }
  }
}



void checkLight() {
  if (state.visibleLight < 290) {
    myLight.on();
  }
  else {
    myLight.off();
  }
}



void startSystem() {
  do {
    state.MoisHumidity = moistureSensor.read_humidity();
    displayText("Moisture: ", state.MoisHumidity , 1 , 0 );
    delay(500);
    state.visibleLight = SI1145.ReadVisible();
    displayText("Sun: ", (int)state.visibleLight, 5, 0);

    if (state.MoisHumidity > 267) {
      break;
    }
    myPump.on();
//        delay(1000);
//        myPump.off();
//        delay(1000);
    checkLight();

  } while (ButtonClickedOff == 0);

  state.MoisHumidity = moistureSensor.read_humidity();
  displayText("Moisture: ", state.MoisHumidity , 1 , 0 );
  delay(500);
  state.visibleLight = SI1145.ReadVisible();
  displayText("Sun: ", (int)state.visibleLight, 5, 0);
}



//void ButtonClick() {
//  if (digitalRead(ButtonPin) == 0) {
//    //delay(10);
//    if (digitalRead(ButtonPin) == 0) {
//      ButtonClicked = true;
//    }
//  }
//}
//
//
//void ButtonClickOff() {
//  if (digitalRead(ButtonPinOff) == 0) {
//    //delay(10);
//    if (digitalRead(ButtonPinOff) == 0) {
//      ButtonClickedOff = true;
//    }
//  }
//}


//Shows readings from all sensors and starts pump with button
void overviewProgram() {
  state.MoisHumidity = moistureSensor.read_humidity();
  displayText("Moisture: ", state.MoisHumidity , 1 , 0 );
  delay(500);
  state.visibleLight = SI1145.ReadVisible();
  displayText("Sun: ", (int)state.visibleLight, 5, 0);

  while ((ButtonClicked == 1) || (ButtonClickedOff == 1)) {
    if (ButtonClicked == 1) {

      // Check Pump and Light
      do {
        state.MoisHumidity = moistureSensor.read_humidity();
        displayText("Moisture: ", state.MoisHumidity , 1 , 0 );
        delay(500);
        state.visibleLight = SI1145.ReadVisible();
        displayText("Sun: ", (int)state.visibleLight, 5, 0);

        startSystem();
        checkLight();
        //displayText("Clicks: ", state.clicks, 2, 0);
        //state.clicks++;
        //state.ButtonClicked = false;
        //state.ButtonClickedOff = false;


        if (ButtonClickedOff == 1) {
          ButtonClicked = 0;
          break;
        }
      } while (state.MoisHumidity < 267);

      // Pump off but check the light
      do {
        state.MoisHumidity = moistureSensor.read_humidity();
        displayText("Moisture: ", state.MoisHumidity , 1 , 0 );
        delay(500);
        state.visibleLight = SI1145.ReadVisible();
        displayText("Sun: ", (int)state.visibleLight, 5, 0);
        myPump.off();
        checkLight();
        //displayText("Clicks: ", state.clicks, 2, 0);
        //state.clicks++;
        //state.ButtonClicked = false;
        //state.ButtonClickedOff = false;
        if (ButtonClickedOff == 1) {
          ButtonClicked = 0;
          break;
        }
      } while (state.MoisHumidity > 267);
    }

    if (ButtonClickedOff == 1) {

      state.MoisHumidity = moistureSensor.read_humidity();
      displayText("Moisture: ", state.MoisHumidity , 1 , 0 );
      delay(500);
      state.visibleLight = SI1145.ReadVisible();
      displayText("Sun: ", (int)state.visibleLight, 5, 0);

      myPump.off();
      myLight.off();

      displayText("Clicks: ", state.clicks, 2, 0);
      state.clicks++;
      ButtonClicked = 0;
      ButtonClickedOff = 0;

    }

  }


  displayText("Sun: ", (int)state.visibleLight, 5, 0);
  displayText("Moisture: ", state.MoisHumidity , 1 , 0 );
  displayText("Encoder: ", state.encodercntr, 3, 0);
  displayText("Waterflow: ", state.WaterflowRate, 4, 0);
  displayText("Sun: ", (int)state.visibleLight, 5, 0);
  displayText("IR: ", (int)state.irLight, 6, 0);
  displayText("UV: ", (int)state.uvLight, 7, 0);
}










//      do {
//        myPump.on();
//        delay(1000);
//        myPump.off();
//        delay(1000);
//        if (state.ButtonClickedOff == true) {
//          state.ButtonClickedOff = true;
//          state.ButtonClicked = false;
//          break;
//        }
//
//        if (state.visibleLight < 290) {
//          myLight.on();
//        }
//        else {
//          myLight.off();
//        }
//      } while (state.ButtonClickedOff = false);
//
//      if (state.ButtonClickedOff == true) {
//        state.ButtonClickedOff = true;
//        state.ButtonClicked = false;
//        break;
//      }
//    }