#include <Wire.h>
#include <SeeedOLED.h>
#include <EEPROM.h>
#include "Arduino.h"
#include <relay.h>
#include "DHT.h"
#include "SI114X.h"
#include "soil_moisture_sensor.h"
#include "GroveEncoder.h"

#define DHTPIN          A0     // what pin we're connected to
#define MoisturePin     A1
#define ButtonPin       2
#define DHTTYPE DHT11   // DHT 11 
#define EncoderPin1     3
#define EncoderPin2     4
#define WaterflowPin    5
#define RelayPin        6

enum Status 
{
    Standby  =  0,
    Warning  =  1,
    Setting   = 2,
    Watering =  3,
};
typedef enum Status Systemstatus;
Systemstatus WorkingStatus;


enum EncoderDir
{
    Anticlockwise = 0,
    Clockwise     = 1,
};
typedef enum EncoderDir EncodedirStatus;
EncodedirStatus EncoderRoateDir;


enum WarningStatus
{
    NoWarning          = 0,
    AirHumidityWarning = 1,
    AirTemperWarning   = 2,
    UVIndexWarning     = 3,
    NoWaterWarning     = 4,
};
typedef enum WarningStatus WarningStatusType;
WarningStatusType SystemWarning;


struct Limens 
{
    unsigned char UVIndex_Limen       = 9;
    unsigned char DHTHumidity_Hi      = 60;
    unsigned char DHTHumidity_Low     = 0;
    unsigned char DHTTemperature_Hi   = 30;
    unsigned char DHTTemperature_Low  = 0;
    unsigned char MoisHumidity_Limen  = 0;
    float         WaterVolume         = 0.2;
};
typedef struct Limens WorkingLimens;
WorkingLimens SystemLimens;

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

Relay relay(RelayPin);
SoilMoistureSensor moistureSensor(MoisturePin);

SI114X SI1145 = SI114X();
DHT dht(DHTPIN, DHTTYPE);

int encodercntr = 0;
void myCallback(int newValue) {
  encodercntr++;
}
GroveEncoder myEncoder(EncoderPin1, &myCallback);

float DHTHumidity    = 0;
float DHTTemperature = 0;
float MoisHumidity   = 100;
float UVIndex        = 0;
char buffer[30];

void setup() {
  Wire.begin();
  SeeedOled.init();  //initialze SEEED OLED display
  SeeedOled.clearDisplay();          //clear the screen and set start position to top left corner
  SeeedOled.setNormalDisplay();      //Set display to normal mode (i.e non-inverse mode)
  SeeedOled.setPageMode();           //Set addressing mode to Page Mode
  SeeedOled.setTextXY(0,0);          //Set the cursor to Xth Page, Yth Column  

  SeeedOled.putString("Smart Greenhouse!");
  
  //encoder.Timer_init();
  
    /* Init DHT11 */
    Serial.begin(9600); 
    Serial.println("Starting up...");
    dht.begin();
    
    /* Init Button */
    pinMode(ButtonPin,INPUT);
    //attachInterrupt(0,ButtonClick,FALLING);
    /* Init Encoder */
    pinMode(EncoderPin1,INPUT);
    pinMode(EncoderPin2,INPUT);
    //attachInterrupt(1,EncoderRotate,RISING);   

    /* Init UV */
    while (!SI1145.Begin()) {
        delay(1000);
    }

    /* Init Water flow */
    pinMode(WaterflowPin,INPUT);
    
    /* Init Relay      */
    pinMode(RelayPin,OUTPUT);
    StartTime = millis();
    WorkingStatus = Standby;
    SystemWarning = NoWarning;
}

// counters for test
int clicks = 0;

void loop() { // main code
  relay.off();

  // Read from the Moisturesensor 
 MoisHumidity   = analogRead(MoisturePin)/7;

// show MoisHuidity on screen
DisplayMoisture();

int buttonState = digitalRead(ButtonPin);

if (buttonState == HIGH) {
        SeeedOled.setTextXY(3,0); 
        sprintf(buffer,"Clicks: %d", ++clicks);
        SeeedOled.putString(buffer);
    }

  SeeedOled.setTextXY(4,0);  //Set the cursor to Xth Page, Yth Column 
  sprintf(buffer,"Encoder: %d", encodercntr);
  SeeedOled.putString(buffer);
  
}

void StandbytoWatering()
{
    SeeedOled.clearDisplay();
    relay.on();
    SeeedOled.setTextXY(0,3);
    SeeedOled.putString("Watering");
    SeeedOled.setTextXY(2,0);
    SeeedOled.putString("FlowRate:");
    SeeedOled.setTextXY(4,0);
    SeeedOled.putString("Volume:");
    StartTime = millis();
}

void DisplayMoisture() 
{
    sprintf(buffer,"Moisture");
    SeeedOled.setTextXY(1,4);          //Set the cursor to Xth Page, Yth Column  
    SeeedOled.putString(buffer);

    sprintf(buffer,"%d.%d%%",(int)(MoisHumidity),(int)((int)(MoisHumidity*100) % 100));
    SeeedOled.setTextXY(2,6);          //Set the cursor to Xth Page, Yth Column  
    SeeedOled.putString(buffer);
}
