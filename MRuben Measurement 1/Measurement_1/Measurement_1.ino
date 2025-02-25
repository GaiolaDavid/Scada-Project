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
  float fuelLevelPercent = (rawFuelLevel / 1023.0) * 100.0;

  // Kiírás a soros monitorra
  Serial.print("Páratartalom: ");
  Serial.print(humidity);
  Serial.print("\t");

  Serial.print("Hőmérséklet: ");
  Serial.print(temperature);
  Serial.println("°C");

  Serial.print("Üzemanyagszint: ");
  Serial.print(fuelLevelPercent);
  Serial.println(" %");

  delay(2000); // 2 másodpercenként frissítjük az adatokat
}

