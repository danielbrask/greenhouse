#include "water_flow_sensor.h"

WaterFlowSensor sensor(2, 1000);
float flow_rate = 0;

void setup(){
	Serial.begin(9600);
}

void loop(){
	flow_rate = sensor.measure_flow_rate();
	Serial.print("Current flow rate: ");
	Serial.print(flow_rate);
	Serial.println(" m^3/s");
	delay(2000);
}