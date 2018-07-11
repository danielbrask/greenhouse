#include "soil_moisture_sensor.h"

SoilMoistureSensor::SoilMoistureSensor(uint8_t analog_pin, int min_humid_level)
{
	_pin = analog_pin;
	_min_humid_level = min_humid_level;
}

int SoilMoistureSensor::read_humidity()
{
	return analogRead(_pin);
}

int SoilMoistureSensor::get_min_humidity_level()
{
	return _min_humid_level;
}

boolean SoilMoistureSensor::check_under_humidity_level()
{
	return read_humidity() < _min_humid_level;
}