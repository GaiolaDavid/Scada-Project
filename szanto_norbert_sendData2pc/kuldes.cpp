#include "Arduino.h"
#include "SoftwareSerial.h"
#include "string.h"

SoftwareSerial rs232Serial(6, 7);

void setup()
{
    Serial.begin(9600);
    rs232Serial.begin(9600);
}

String readRS232Line()
{
    String line = "";
    char character;
    bool startLine = false;

    pinMode(1, INPUT);

    while (true)
    {
        if (rs232Serial.available())
        {

            character = rs232Serial.read();
            if (character == ':')
            {
                startLine = true;
                line += character;
            }
            else if (character == '\n' && startLine)
            {
                return line;
            }
            else if (startLine)
            {
                line += character;
            }
        }
    }
    return line;
}

void writeRS232Line(String line)
{
    for (int i = 0; i < line.length(); i++)
    {
        rs232Serial.write(line[i]);
    }
    rs232Serial.write('\n');
}

void loop()
{
    if (rs232Serial.available())
    {
        String receivedLine = readRS232Line();
        if (receivedLine.length() > 0)
        {
            Serial.print("Received Line: ");
            Serial.println(receivedLine);
        }
    }

    if (Serial.available())
    {
        String lineToSend = "Str";
        if (lineToSend.length() > 0)
        {
            writeRS232Line(lineToSend);
            Serial.print("Sent Line: ");
            Serial.println(lineToSend);
        }
    }
    delay(10);
}

