#include <SoftwareSerial.h>
#define rxPin 11
#define txPin 10
#define enPin 13
#define inverse_logic false
int counter = 0;
SoftwareSerial serial485 = SoftwareSerial(rxPin, txPin, inverse_logic);

void setup() {
  pinMode(enPin, OUTPUT);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  digitalWrite(enPin,HIGH);
  serial485.begin(9600);
  Serial.begin(57600);
  //serial485.write("Test");
  Serial.println("Test");
  serial485.write(":RUbEnaBA\n\r");
  

}

void loop() {
  
  
  
  

}
