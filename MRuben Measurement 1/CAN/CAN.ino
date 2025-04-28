#include <SPI.h>
#include <Wire.h>
#include <mcp2515.h>
#include <DHT11.h>

struct can_frame canMsg;

float recovered1;
float recovered2;

void createMessage(can_frame *message, float num1, float num2) {
    memcpy(message->data, &num1, sizeof(float));
    memcpy(message->data + sizeof(float), &num2, sizeof(float));
}

void getData(can_frame *message, float *num1, float *num2) {
    memcpy(num1, message->data, sizeof(float));
    memcpy(num2, message->data + sizeof(float), sizeof(float));
}


MCP2515 mcp2515(53);

void setup() {
  while (!Serial);
  Serial.begin(115200);
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode();
}

int temp = 12;
int humi = 24;

char buffer[32];
char numChar[6];

void loop() {

  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    getData(&canMsg, &recovered1, &recovered2);
    dtostrf(recovered1, 4, 2, numChar);
    snprintf(buffer, sizeof(buffer), "recovered1: %s\n", numChar);
    Serial.print(buffer);
    dtostrf(recovered2, 4, 2, numChar);
    snprintf(buffer, sizeof(buffer), "recovered2: %s\n", numChar);
    Serial.print(buffer);
  }

}