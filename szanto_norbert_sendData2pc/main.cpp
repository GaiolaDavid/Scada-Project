#include <Arduino.h>
#include "math.h"

void send_all_data(float temperature, float humidity, float fuel_level, int throttle, int ultrasonic);

void send_package(char type, float number, int precision, int year, int month, int day, int hour, int minute, int second);

struct Message
{
  char type;
  double value;
  int precision;
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
};

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  double temperature; // xy.ab
  double humidity;    // xy.ab
  double fuel_level;  // xyz.a

  int throttle;   // sxyz
  int ultrasonic; // xyz

  temperature = 25.72;
  humidity = 13.12;
  fuel_level = 90.1;

  throttle = 1234;
  ultrasonic = 987;

  send_all_data(temperature, humidity, fuel_level, throttle, ultrasonic);
}

void send_package(char type, float number, int precision, int year, int month, int day, int hour, int minute, int second)
{
  char formated_c[25];
  int a = number;

  int p = 1;
  int g = 1;
  for (int i = 0; i < precision; ++i)
    p *= 10;
  for (int i = 0; i < 4 - precision; ++i)
    g *= 10;

  int b = int(number * p) % p * g;

  sprintf(formated_c, ":%c%04d%04d%04d%02d%02d%02d%02d%02d\n", type, a, b, year, month, day, hour, minute, second);
  Serial.print(formated_c);
}

void send_all_data(float temperature, float humidity, float fuel_level, int throttle, int ultrasonic)
{
  send_package('t', temperature, 2, 2025, 3, 11, 15, 51, 1);
  send_package('h', humidity, 2, 2025, 3, 11, 15, 51, 2);
  send_package('f', fuel_level, 1, 2025, 3, 11, 15, 51, 3);
  send_package('r', throttle, 0, 2025, 3, 11, 15, 51, 4);
  send_package('u', ultrasonic, 0, 2025, 3, 11, 15, 51, 5);
}