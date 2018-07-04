#include <Wire.h>
#include <SeeedOLED.h>
#include <EEPROM.h>
#include "Arduino.h"
#include <relay.h>
#include "DHT.h"
#include "SI114X.h"

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
SI114X SI1145 = SI114X();
DHT dht(DHTPIN, DHTTYPE);
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
  //    encoder.Timer_init();
  
    /* Init DHT11 */
    Serial.begin(9600); 
    Serial.println("Starting up...");
    dht.begin();
    
    /* Init Button */
    pinMode(ButtonPin,INPUT);
    attachInterrupt(0,ButtonClick,FALLING);
    /* Init Encoder */
    pinMode(EncoderPin1,INPUT);
    pinMode(EncoderPin2,INPUT);
    attachInterrupt(1,EncoderRotate,RISING);   

    /* Init UV */
    while (!SI1145.Begin()) {
        delay(1000);
    }

    /* Init Water flow */
    pinMode(WaterflowPin,INPUT);
    
    /* Init Relay      */
    pinMode(RelayPin,OUTPUT);
    /* The First time power on to write the default data to EEPROM */
    if (EEPROM.read(EEPROMAddress) == 0xff) {
        EEPROM.write(EEPROMAddress,0x00);
        EEPROM.write(++EEPROMAddress,SystemLimens.UVIndex_Limen);
        EEPROM.write(++EEPROMAddress,SystemLimens.DHTHumidity_Hi);
        EEPROM.write(++EEPROMAddress,SystemLimens.DHTHumidity_Low);
        EEPROM.write(++EEPROMAddress,SystemLimens.DHTTemperature_Hi);
        EEPROM.write(++EEPROMAddress,SystemLimens.DHTTemperature_Low);
        EEPROM.write(++EEPROMAddress,SystemLimens.MoisHumidity_Limen);
        EEPROM.write(++EEPROMAddress,((int)(SystemLimens.WaterVolume*100))/255);    /*  */
        EEPROM.write(++EEPROMAddress,((int)(SystemLimens.WaterVolume*100))%255);
    } else { /* If It's the first time power on , read the last time data */
        EEPROMAddress++;
        SystemLimens.UVIndex_Limen      = EEPROM.read(EEPROMAddress++);
        SystemLimens.DHTHumidity_Hi     = EEPROM.read(EEPROMAddress++);
        SystemLimens.DHTHumidity_Low    = EEPROM.read(EEPROMAddress++);
        SystemLimens.DHTTemperature_Hi  = EEPROM.read(EEPROMAddress++);
        SystemLimens.DHTTemperature_Low = EEPROM.read(EEPROMAddress++);
        SystemLimens.MoisHumidity_Limen = EEPROM.read(EEPROMAddress++);
        SystemLimens.WaterVolume =   (EEPROM.read(EEPROMAddress++)*255 + EEPROM.read(EEPROMAddress))/100.0;
    }
    
    StartTime = millis();
    WorkingStatus = Standby;
    SystemWarning = NoWarning;
}


void loop() { // main code
  relay.off();
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
void ButtonClick()
{
    
    if(digitalRead(ButtonPin) == 0){
        delay(10);
        if(digitalRead(ButtonPin) == 0){
            ButtonFlag = 1;
        }
    }  
}


void EncoderRotate()
{
    if(digitalRead(EncoderPin1) == 1) {
        delay(10);
        if(digitalRead(EncoderPin1) == 1) {
            if(EncoderFlag == 0) {
                EncoderFlag = 1;
                if(digitalRead(EncoderPin2) == 1) {
                    EncoderRoateDir = Clockwise;
                } else {
                    EncoderRoateDir = Anticlockwise;
                }
           }
        } else {
        }
    } else {
    }
}

