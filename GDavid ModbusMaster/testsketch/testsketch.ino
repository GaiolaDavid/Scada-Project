char incomingByte = 0;
void setup() {
  
  Serial.begin(57600);

}

void loop() {
  if (Serial.available() > 0) {
        // read the incoming byte:
        incomingByte = Serial.read();

        // say what you got:
        Serial.print("I received: ");
        Serial.println(incomingByte, DEC);
        Serial.println(incomingByte);
      }

}
