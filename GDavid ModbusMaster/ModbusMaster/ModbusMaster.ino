
// CONNECTIONS:
// DS1302 CLK/SCLK --> 5
// DS1302 DAT/IO --> 4
// DS1302 RST/CE --> 2
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND

#include <RtcDS1302.h>

ThreeWire myWire(4,5,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

#include <SoftwareSerial.h>
#define rxPin 11
#define txPin 10
#define enPin 13
#define inverse_logic false
int counter = 0;
int cycle=0;
bool rw = 0;
SoftwareSerial serial485 = SoftwareSerial(rxPin, txPin, inverse_logic);
char data485[30]="";

#define countof(a) (sizeof(a) / sizeof(a[0]))

const char data[] = "what time is it";


void writer(int e, int f,char* datain) {
  digitalWrite(enPin,HIGH);
  char temp[30] =":";
  char tempc[1]="";
  sprintf(tempc,"%i",e);
  strcat(temp,tempc);
  sprintf(tempc,"%i",f);
  strcat(temp,tempc);
  strcat(temp,datain);
  strcat(temp,"\n\r");
  //Serial.println(temp);
  serial485.write(temp);
  digitalWrite(enPin,LOW);
}

void reader(char* ptr) {
  Serial.println("Reading");
  digitalWrite(enPin,LOW);
  bool flag,flag1,flag2=false;
  char string[30]="";
  bool read=false;
  while(!read) {
    if(serial485.available()) {
        int curreadi=serial485.read();
        char curread=(char)curreadi;
        if(flag) {
          strncat(string,&curread,1);
          if(curreadi==10) {
            flag1=true;
          }
          if(flag1 && curreadi == 13) {
            flag1=false;
            flag=false;
            flag2=true;
          }
        }
        if(curreadi==58) {
          flag=true;
          strcpy(string,"");
          Serial.println(string);
        }
      }
      if(flag2) {
        flag2=false;
        read=true;
      }
    }
  strcpy(ptr,string);
}

void setup () 
{
    Serial.begin(57600);

    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);
    
    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    Serial.println();

    if (!Rtc.IsDateTimeValid()) 
    {
        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }


/* comment out on a second run to see that the info is stored long term */
    // Store something in memory on the RTC
    uint8_t count = sizeof(data);
    uint8_t written = Rtc.SetMemory((const uint8_t*)data, count); // this includes a null terminator for the string
    if (written != count) 
    {
        Serial.print("something didn't match, count = ");
        Serial.print(count, DEC);
        Serial.print(", written = ");
        Serial.print(written, DEC);
        Serial.println();
    }
/* end of comment out section */
}

void loop () 
{
  {
    RtcDateTime now = Rtc.GetDateTime();

    printDateTime(now);

    if (!now.IsValid())
    {
        // Common Causes:
        //    1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }

    delay(5000);

    // read data
    uint8_t buff[20];
    const uint8_t count = sizeof(buff);
    // get our data
    uint8_t gotten = Rtc.GetMemory(buff, count);

    if (gotten != count) 
    {
        Serial.print("something didn't match, count = ");
        Serial.print(count, DEC);
        Serial.print(", gotten = ");
        Serial.print(gotten, DEC);
        Serial.println();
    }

    Serial.print("data read (");
    Serial.print(gotten);
    Serial.print(") = \"");
    // print the string, but terminate if we get a null
    for (uint8_t ch = 0; ch < gotten && buff[ch]; ch++)
    {
        Serial.print((char)buff[ch]);
    }
    Serial.println("\"");
  }
    switch(cycle) {
      case 1:
      writer(1,0,"");
      reader(data485);
      Serial.println(data485);
      break;
      default:
      break;
    }





    delay(5000);
}



void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%04u/%02u/%02u %02u:%02u:%02u"),
            dt.Year(),
            dt.Month(),
            dt.Day(),
            dt.Hour(),
            dt.Minute(),
            dt.Second());
    Serial.println(datestring);
}

