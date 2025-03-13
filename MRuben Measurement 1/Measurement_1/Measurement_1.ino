#include <DHT.h>

#define DHTPIN 2 //a homersekletszenzornak a laba a 2-es porton van
#define DHTTYPE DHT11   

#define FUEL_SENSOR_PIN A0

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(FUEL_SENSOR_PIN, INPUT);
  Serial.begin(9600);
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

  String tempString = String(tempInt) +" "+ String(tempDec);
  String humString = String(humInt) +" "+ String(humDec);
  String fuelString = String(fuelInt) +" "+ String(fuelDec);

   Serial.print("Páratartalom: ");
    Serial.print(humString);
    Serial.print(" %\t");

    Serial.print("Hőmérséklet: ");
    Serial.print(tempString);
    Serial.print(" °C\t");

    Serial.print("Üzemanyagszint: ");
    Serial.print(fuelString);
    Serial.println(" %");

  delay(2000); // 2 másodpercenként frissítjük az adatokat
}

