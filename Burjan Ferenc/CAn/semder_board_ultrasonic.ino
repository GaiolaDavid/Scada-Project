#include <SPI.h>
#include <Wire.h>

#include <mcp2515.h>

#define OLED_RESET 4


struct can_frame canMsg1;
struct can_frame canMsg2;
MCP2515 mcp2515(53);



const int trigPin = 5;
const int echoPin = 6;


void createMessage(can_frame *message, float num1, float num2) {
    memcpy(message->data, &num1, sizeof(float));
    memcpy(message->data + sizeof(float), &num2, sizeof(float));
}

void getData(can_frame *message, float *num1, float *num2) {
    memcpy(num1, message->data, sizeof(float));
    memcpy(num2, message->data + sizeof(float), sizeof(float));
}

void setup() {
  Serial.begin(9600); // Initialize serial communication
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode();
}
void loop() {
  float distance = 1121.21;
  float duration = 5.21;
  
  canMsg1.can_id  = 0x03;
  canMsg1.can_dlc = 8;
  createMessage(&canMsg1, distance, duration);

  mcp2515.sendMessage(&canMsg1);
  //mcp2515.sendMessage(&canMsg2);
  delay(200);
  Serial.println("Messages sent");
}
