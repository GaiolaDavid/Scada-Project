// HC-SR04 Ultrahangos érzékelő beállításai
const int trigPin = 9;
const int echoPin = 6;  // Pin 10 helyett 6-ra módosítva
long duration;
int distance;

// HW-040 enkóder beállításai
const int pinA = 2;   // DT
const int pinB = 3;   // CLK
volatile int position = 0;
int lastStateA;
int currentStateA;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  lastStateA = digitalRead(pinA);
  attachInterrupt(digitalPinToInterrupt(pinA), readEncoder, CHANGE);
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
  if (Serial.available()) {
    String receivedData = Serial.readStringUntil('\n');
    if (receivedData.startsWith(":2")) {  // Ellenőrizzük, hogy nekünk szól-e
      char functionCode = receivedData.charAt(2);
      if (functionCode == '0') {
        sendEncoderData();
      } else if (functionCode == '1') {
        sendDistanceData();
      }
    }
  }
}

void sendEncoderData() {
  bool negative = (position < 0);
  long absPosition = abs(position);
  String response = ":50";
  response += negative ? "1" : "0";
  response += String(absPosition / 100).charAt(0);
  response += String((absPosition / 10) % 10);
  response += String(absPosition % 10);
  response += "FRLF";
  Serial.println(response);
}

void sendDistanceData() {
  getDistance();
  String response = ":51";
  response += String((distance / 100) % 10);
  response += String((distance / 10) % 10);
  response += String(distance % 10);
  response += "FRLF";
  Serial.println(response);
}