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
  getDistance();
  sendDataAndPrint();
  delay(500);
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

void sendDataAndPrint() {
  bool negative = (position < 0);
  long absPosition = abs(position);

  String dataString = String(negative) +
                      String(absPosition / 100).charAt(0) +
                      String((absPosition / 10) % 10) +
                      String(absPosition % 10) +
                      String((distance / 100) % 10) +
                      String((distance / 10) % 10) +
                      String(distance % 10);

  Serial.println(dataString);
}
