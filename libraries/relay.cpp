#include "relay.h"

Relay::Relay(uint8_t pin)
{
	_pin = pin;
}

void Relay::on()
{
	digitalWrite(_pin, HIGH);
}

void Relay::off()
{
	digitalWrite(_pin, LOW);
}