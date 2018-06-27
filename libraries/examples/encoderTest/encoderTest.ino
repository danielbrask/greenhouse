#include <GroveEncoder.h>

void myCallback(int newValue) {
  Serial.print(newValue, HEX);
  Serial.print("\n");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  GroveEncoder myEncoder(7, &myCallback);
  while(1)
  {
    delay(1000);

    Serial.print("\nLoop... ");
  }
}
