#include <SoftwareSerial.h>
#include <String.h>
#define rxPin 11
#define txPin 10
#define enPin 13
#define inverse_logic false

SoftwareSerial serial485 = SoftwareSerial(rxPin, txPin, inverse_logic);
bool flag= false;
bool flag1=false;
bool flag2=false;
char string[30]="";
void setup() {
  pinMode(enPin, OUTPUT);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  digitalWrite(enPin,HIGH);
  serial485.begin(9600);
  Serial.begin(57600);
  Serial.println("START");
  serial485.print(":TEST\n\r");
}

void loop() {
  // if(serial485.available()) {
  //   int curreadi=serial485.read();
  //   char curread=(char)curreadi;
  //   if(flag) {
  //     strncat(string,&curread,1);
  //     if(curreadi==10) {
  //       flag1=true;
  //     }
  //     if(flag1 && curreadi == 13) {
  //       flag1=false;
  //       flag=false;
  //       flag2=true;
  //       Serial.println("String got");
  //     }
  //   }
  //   if(curreadi==58) {
  //     flag=true;
  //     strcpy(string,"");
  //     Serial.println("String read start");
  //     Serial.println(string);
  //   }
  // }
  // if(flag2) {
  //   Serial.println("String is");
  //   Serial.print(string);
  //   flag2=false;
  // }

}
