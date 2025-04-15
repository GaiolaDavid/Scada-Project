#include <Arduino.h>
#include <string.h>
#include <mcp2515.h>

struct can_frame canMsg1;
MCP2515 mcp2515(10);

float num1 = 12;
float num2 = 532.2;

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

void setup() {
    Serial.begin(9600);
}

void loop() {
    Serial.println("Messages sent");
    
    canMsg1.can_id  = 0x0F6;
    canMsg1.can_dlc = 8;

    createMessage(&canMsg1, num1, num2);
    getData(&canMsg1, &recovered1, &recovered2);

    char buffer[32];
    char numChar[6];
    dtostrf(recovered1, 4, 2, numChar);
    snprintf(buffer, sizeof(buffer), "recovered1: %s\n", numChar);
    Serial.print(buffer);
    dtostrf(recovered2, 4, 2, numChar);
    snprintf(buffer, sizeof(buffer), "recovered2: %s\n", numChar);
    Serial.print(buffer);

    delay(500);
  }

// int main() {

//     uint8_t bytes[8];
//     float recovered1, recovered2;

//     canMsg1.can_id  = 0x0F6;
//     canMsg1.can_dlc = 8;
//     canMsg1.data[0] = 0x8E;
//     canMsg1.data[1] = 0x87;
//     canMsg1.data[2] = 0x32;
//     canMsg1.data[3] = 0xFA;
//     canMsg1.data[4] = 0x26;
//     canMsg1.data[5] = 0x8E;
//     canMsg1.data[6] = 0xBE;
//     canMsg1.data[7] = 0x86;

//     createMessage(bytes, num1, num2);
//     getData(bytes, &recovered1, &recovered2);
    
//     // printf("Number1: %.2f\n", num1);
//     // printf("Recovered float: %.2f\n", recovered1);

//     // printf("Number2: %.2f\n", num2);
//     // printf("Recovered float: %.2f\n", recovered2);
// }