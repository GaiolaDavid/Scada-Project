#include <SoftwareSerial.h>
#define rxPin 11
#define txPin 10
#define enPin 13
#define inverse_logic false
int counter = 0;
bool rw = 1;
SoftwareSerial serial485 = SoftwareSerial(rxPin, txPin, inverse_logic);
char data[30]="";

void writer(int e, int f,char* datain) {
  digitalWrite(enPin,HIGH);
  char temp[30] =":";
  char tempc[1]="";
  sprintf(tempc,"%i",e);
  strcat(temp,tempc);
  sprintf(tempc,"%i",f);
  strcat(temp,tempc);
  strcat(temp,datain);
  strcat(temp,"\n\r");
  Serial.println(temp);
  serial485.write(temp);
  digitalWrite(enPin,LOW);
}

void reader(char* ptr) {
  Serial.println("Reading");
  digitalWrite(enPin,LOW);
  bool flag,flag1,flag2=false;
  char string[30]="";
  bool read=false;
  while(!read) {
    if(serial485.available()) {
        int curreadi=serial485.read();
        char curread=(char)curreadi;
        if(flag) {
          strncat(string,&curread,1);
          if(curreadi==10) {
            flag1=true;
          }
          if(flag1 && curreadi == 13) {
            flag1=false;
            flag=false;
            flag2=true;
          }
        }
        if(curreadi==58) {
          flag=true;
          strcpy(string,"");
          //Serial.println(string);
        }
      }
      if(flag2) {
        flag2=false;
        read=true;
      }
    }
  strcpy(ptr,string);
}

void setup() {
  pinMode(enPin, OUTPUT);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  serial485.begin(9600);
  Serial.begin(57600);
  Serial.println("Test");
  digitalWrite(enPin,LOW);
  writer(1,0,"");
  reader(data);
  Serial.println(data);
}

void loop() {
  
}
