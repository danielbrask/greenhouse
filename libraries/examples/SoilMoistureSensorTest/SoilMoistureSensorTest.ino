#include "soil_moisture_sensor.h"

SoilMoistureSensor soil_sensor(A0, min_humid_soil);
int humidity_level;

void setup()
{
	Serial.begin(9600);
}

void loop()
{
	humidity_level = soil_sensor.read_humidity();
	Serial.print("Current soil level: ");
	Serial.println(humidity_level);
	if(soil_sensor.check_under_humidity_level()){
		Serial.print("Humidity level above acceptable level ");
		Serial.println(soil_sensor.get_min_humidity_level());
	}
}