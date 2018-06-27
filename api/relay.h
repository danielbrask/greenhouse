#ifndef __RELAY__
#define __RELAY__

#include"Arduino.h"

class Relay{
	public:
		Relay(uint8_t pin);
		
		void on();
		void off();
	private:
		uint8_t _pin;
};

#endif