#ifndef __WATERFLOWSENSOR__
#define __WATERFLOWSENSOR__

#include <Arduino.h>

// Having this value as global is bad since there will be problem if there is more 
// then one sensor used. Looking for an alternative would be preferred.
extern unsigned counted_interrupts;

void interrupt_iterator();

class WaterFlowSensor{
	public:
		// pin: is the pin that the sensor is connected to, this needs to be a pin that 
		// can be used for interrupts, on Arduino Uno this would be pin 2 and 3. Se
		// https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
		// for more information
		// read_time: the amount of time that the sensor will take
		// to perform is reading in milliseconds
		WaterFlowSensor(uint8_t pin, unsigned long read_time=1000);
		float measure_flow_rate();
	private:
		float calculate_flow();
	
		uint8_t _pin, _interrupt_pin;
		unsigned long _read_time;
		
		// The value 7.5 should be of the unit 1/liter, this conversion should give wave_volume the unit m^3
		const float wave_volume = 1000/7.5;
};

#endif