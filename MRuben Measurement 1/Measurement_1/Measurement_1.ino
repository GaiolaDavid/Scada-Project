#include <DHT.h>

#define DHTPIN 2 //a homersekletszenzornak a laba a 2-es porton van
#define DHTTYPE DHT11   

#define FUEL_SENSOR_PIN A0

DHT dht(DHTPIN, DHTTYPE);

#include <SoftwareSerial.h>
#define rxPin 11
#define txPin 10
#define enPin 13
#define inverse_logic false
int counter = 0;
bool rw = 1;
SoftwareSerial serial485 = SoftwareSerial(rxPin, txPin, inverse_logic);
char data485[30]="";

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
  //Serial.println(temp);
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
          Serial.println(string);
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
  //Serial.println("Test");
  digitalWrite(enPin,LOW);

  pinMode(FUEL_SENSOR_PIN, INPUT);
  Serial.begin(57600);
  Serial.println("DHT11 hőmérséklet,páratartalom és üzemanyag mérés indítása...");
  dht.begin();

}

void loop() {
  // beolvasom a szenzor értékeit
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Hiba az érzékelő adatainak beolvasásakor!");
      return;
  }

  int rawFuelLevel = analogRead(FUEL_SENSOR_PIN);
  float fuelLevelPercent = (rawFuelLevel / 1023.0) * 100.0 ;

  int tempInt = (int)temperature;
  int tempDec = (int)(temperature * 10) % 100;

  int humInt = (int)humidity;
  int humDec = (int)(humidity * 10) % 100;

  int fuelInt = (int)fuelLevelPercent;
  int fuelDec = (int)(fuelLevelPercent * 10) % 10;

  String tempString = String(tempInt) +"."+ String(tempDec);
  String humString = String(humInt) +"."+ String(humDec);
  String fuelString = String(fuelInt) +"."+ String(fuelDec);

  char tempchararray[30];
  strcpy(tempchararray,tempString.c_str());
  Serial.println("TempCharArrayTest");
  Serial.println(tempchararray);

  char humiditychararray[30];
  strcpy(humiditychararray,humString.c_str());
  Serial.println("HumidityCharArrayTest");
  Serial.println(humiditychararray);

  char fuelchararray[30];
  strcpy(fuelchararray,fuelString.c_str());
  Serial.println("FuelCharArrayTest");
  Serial.println(fuelchararray);

  if(serial485.available()) {
    if(serial485.peek()==58) {
      reader(data485);
      Serial.println(data485);
      Serial.print(data485[0]);
      if(data485[0]=='1') {
        Serial.println("Message for me");
        char temp = data485[1];
        char arraytosend[30];
        int funcreturn;
        if(temp=='0') {
          Serial.println(temp);
          Serial.println("Requested Temp");
          funcreturn=0;
          sprintf(arraytosend,"%s",tempchararray);
          Serial.println(tempchararray);
          
        }
        if(temp=='1') {
          Serial.println("Requested Hum");
          funcreturn=1;
          sprintf(arraytosend,"%s",humiditychararray);
          Serial.println(humiditychararray);
        }
        writer(5,funcreturn,tempchararray);
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


  delay(2000); // 2 másodpercenként frissítjük az adatokat
}

