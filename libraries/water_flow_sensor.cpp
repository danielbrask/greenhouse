#include "water_flow_sensor.h"

unsigned counted_interrupts = 0;

void interrupt_iterator()
{
	++counted_interrupts;
}

WaterFlowSensor::WaterFlowSensor(uint8_t pin, unsigned long read_time)
{
	_pin = pin;
	_interrupt_pin = digitalPinToInterrupt(_pin);
	_read_time = read_time;
	pinMode(_pin, INPUT);
}

float WaterFlowSensor::measure_flow_rate()
{
	counted_interrupts = 0;
	attachInterrupt(_interrupt_pin, interrupt_iterator, RISING);
	delay(_read_time);
	float flow = calculate_flow();
	detachInterrupt(_interrupt_pin);
	return flow;
}

// flow_rate = volume * frequency [m^3/s]
// frequency = 1000 * counted_interrupts/read_time[ms] = [1/s]
// volume = wave_volume[m^3] (this might need confirmation but this seems to be consistent with equations)
float WaterFlowSensor::calculate_flow()
{
	return 1000*counted_interrupts*wave_volume/_read_time;
}