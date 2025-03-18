// Motor vezérlő pine
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

int rpm = 10;  // Kezdeti RPM
int speedDelay;
bool direction = true; // true = előre, false = hátra

// Motor lépésminták (fél lépéses üzemmód)
int motorSteps[8][4] = {
  {1, 0, 0, 0}, 
  {1, 1, 0, 0}, 
  {0, 1, 0, 0}, 
  {0, 1, 1, 0}, 
  {0, 0, 1, 0}, 
  {0, 0, 1, 1}, 
  {0, 0, 0, 1}, 
  {1, 0, 0, 1}
};

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.begin(9600);   // Soros Monitorhoz kommunikáció
  Serial.println("Írj be egy parancsot (pl. :301E) és nyomj Entert!");

  updateSpeed();
}

void loop() {
  receiveData(); // Serial Monitorból beolvasás

  for (int i = 0; i < 2048; i++) {
    int stepIndex = direction ? (i % 8) : (7 - (i % 8));
    stepMotor(stepIndex);
    delay(speedDelay);
  }
}

void receiveData() {
  if (Serial.available()) {
    String received = Serial.readStringUntil('\n');  // Üzenet beolvasása

    // Formátum ellenőrzése :30XX és :31XX esetén
    if (received.length() >= 6 && received[0] == ':' && (received[1] == '3' && (received[2] == '0' || received[2] == '1'))) {
      String hexRpm = received.substring(3, 5);  // HEX RPM kiolvasása
      int newRpm = strtol(hexRpm.c_str(), NULL, 16);  // HEX átalakítás decimálissá
      
      if (newRpm >= 0 && newRpm <= 0x0F) {  // RPM csak 0x0F-ig engedélyezett
        rpm = newRpm;
        direction = (received[2] == '0'); // '0' esetén előre, '1' esetén hátra
        updateSpeed();
        Serial.print("Új RPM beállítva: ");
        Serial.println(rpm);
        Serial.print("Irány: ");
        Serial.println(direction ? "Előre" : "Hátra");
        Serial.print(":5");
        Serial.print(direction ? "0" : "1");
        Serial.println("F0CRLF"); // Sikeres beállítás válaszüzenet az irány szerint
      } else {
        Serial.println("Érvénytelen RPM érték! Csak 00 és 0F között adhatsz meg értéket.");
        Serial.print(":5");
        Serial.print(received[2]);
        Serial.println("0FCRLF"); // Sikertelen beállítás válaszüzenet az irány szerint
      }
    } else {
      Serial.println("Hibás formátum! Írj be egy üzenetet pl.: :300F vagy :310F");
      Serial.println(":500FCRLF"); // Hibás formátum esetén sikertelen válasz
    }
  }
}

void updateSpeed() {
  speedDelay = (60L * 1000) / (rpm * 2048);
}

void stepMotor(int step) {
  digitalWrite(IN1, motorSteps[step][0]);
  digitalWrite(IN2, motorSteps[step][1]);
  digitalWrite(IN3, motorSteps[step][2]);
  digitalWrite(IN4, motorSteps[step][3]);
}