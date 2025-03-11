#include <SoftwareSerial.h>
#define rxPin 11
#define txPin 10
#define inverse_logic false

void setup() {
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  SoftwareSerial serial485(rxPin, txPin, inverse_logic);
  serial485.begin(115200);
  


}

void loop() {
  

}
