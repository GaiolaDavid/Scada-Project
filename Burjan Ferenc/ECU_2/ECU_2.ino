// HC-SR04 Ultrahangos érzékelő beállításai
const int trigPin = 9;
const int echoPin = 6;  // Pin 10 helyett 6-ra módosítva

#include <SoftwareSerial.h>
#define rxPin 11
#define txPin 10
#define enPin 13
#define inverse_logic false

bool rw = 1;
SoftwareSerial serial485 = SoftwareSerial(rxPin, txPin, inverse_logic);
char data485[30]="";
long duration;
int distance;

// HW-040 enkóder beállításai
const int pinA = 2;   // DT
const int pinB = 3;   // CLK
volatile int position = 0;
int lastStateA;
int currentStateA;

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
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  pinMode(enPin, OUTPUT);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  serial485.begin(9600);
  Serial.begin(57600);
  digitalWrite(enPin,LOW);
  lastStateA = digitalRead(pinA);
  attachInterrupt(digitalPinToInterrupt(pinA), readEncoder, CHANGE);
  Serial.println("Started ECU2");
}

void loop() {
  listenForMaster();
  delay(100);
}

void getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0344 / 2;
  if (distance > 999) distance = 999; // Limitáljuk a maximális távolságot 999 cm-re
}

void readEncoder() {
  currentStateA = digitalRead(pinA);
  if (currentStateA != lastStateA) {
    if (digitalRead(pinB) != currentStateA) {
      position++;
    } else {
      position--;
    }
  }
  lastStateA = currentStateA;
}

void listenForMaster() {
  if(serial485.available()) {
    if(serial485.peek()==58) {
      reader(data485);
      Serial.println(data485);
      Serial.print(data485[0]);
      if(data485[0]=='2') {
        Serial.println("Message for me");
        char temp = data485[1];
        char arraytosend[30];
        int funcreturn;
        if(temp=='0') {
          Serial.println("Requested Trottle");
          funcreturn=0;

          char temparray[30];
          sprintf(temparray,"%i%i",currentStateA,abs(position));
          sprintf(arraytosend,"%s",temparray);
          
        }
        if(temp=='1') {
          Serial.println("Requested Distance");
          funcreturn=1;
          char temparray[30];
          sprintf(temparray,"%i",distance);
          
          sprintf(arraytosend,"%s",temparray);
        }

        Serial.println(arraytosend);
        writer(5,funcreturn,arraytosend);
        Serial.println("Message Sent");
      }
    } else {
      Serial.println("Clearing Buffer");
      while(serial485.peek()!=58 && serial485.available()) {
        Serial.println((char)serial485.read());
      }
      Serial.println("Cleared buffer");
    }
  }
  getDistance();
  readEncoder();

}
