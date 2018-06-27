#include "relay.h"

Relay relay(4);

void setup()
{
	
}

void loop()
{
	relay.on();
	delay(1000);
	relay.off();
	delay(1000);
}