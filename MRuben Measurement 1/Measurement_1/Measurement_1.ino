#include <DHT.h>

#define DHTPIN 2 //a homersekletszenzornak a laba a 2-es porton van
#define DHTTYPE DHT11   

#define FUEL_SENSOR_PIN A0

DHT dht(DHTPIN, DHTTYPE);

void setup() {
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

void writer(int e, int f,int data) {
  digitalWrite(enPin,HIGH);
  serial485.write(":TESTSTRING\n\r");
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
   

  delay(2000); // 2 másodpercenként frissítjük az adatokat
}

