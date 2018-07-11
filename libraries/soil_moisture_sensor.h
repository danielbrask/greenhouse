#ifndef __SOIL_MOISTURE_SENSOR__
#define __SOIL_MOISTURE_SENSOR__

#include <Arduino.h>

const int min_humid_soil = 300;
const int min_in_water = 700;

class SoilMoistureSensor{
	public:
		SoilMoistureSensor(uint8_t analog_pin, int min_humid_level=min_humid_soil);
		
		int read_humidity();
		boolean check_under_humidity_level();
		int get_min_humidity_level();
	private:
		uint8_t _pin;
		int _min_humid_level;
	
};

#endif