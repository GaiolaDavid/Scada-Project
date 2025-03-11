//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <IRremote.hpp>
#include <Keypad.h>

const int RECV_PIN = 2;  // Az IR szenzor DATA lábát a 2-es pinre kötöttük

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

const byte ROWS = 4; // 4 sor
const byte COLS = 4; // 4 oszlop

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {11, 10, 9, 8}; // Sorokhoz csatlakoztatott Arduino lábak
byte colPins[COLS] = {7, 6, 5, 4}; // Oszlopokhoz csatlakoztatott Arduino lábak

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


//ECU 1  szenzorok - kimenetek
float temp=24.15; //4 byte
float humidity=78.24; //4 byte
float fuel_level = 45.55; //4 byte
//12 byte

//ECU 2   szenzorok - kimenetek
float throttle = 55.55; //sxy.ab (s + vagy -)     5 byte
float ultrasonic = 12.12; // 4
//9 byte

//ECU 3   motor - bemenet  (PC tol a gepbe)
int rpm = 113; // 3 byte

//ECU 4 bemenet-kimenet
//dashboard x-screen nr = ecu nr  --1 byte       ezt a dashboardtol kapja 
//lcd display all data from /to ecu1, ecu2, ecu3  -- 24 byte        ezt a halozattol kapja
//beeper x / 1 byte 1-on 0-off          ezt is a halozattol kapja
//t0bb virtualis screen


//ECU 5 (master)
//RTC (real time clock) - time-stamp data 
//ecu data: ecu1 - 12 byte; ecu2 - 9 byte; ecu3 - 3 byte; ecu4 1 byte; összesen 39 byte, ezt kell a matlab ábrázolja


//MODBUS

void setup()
{
  //távirányító vevő:
  Serial.begin(9600); //ennek egyezni kell a serial monitor baudrate-el
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);  // IR vevő indítása
  Serial.println("IR vevő kész..."); //(infravörös távirányító - IR infrared remote)

  //LCD kijelző inicializálás 
  lcd.init();              
  lcd.backlight();

  // Alapértelmezett üzenet
  lcd.setCursor(4,0);
  lcd.print("IR ready");
}

void loop()
{
    if (IrReceiver.decode()) {  // Ha érkezik IR jel
    int command = IrReceiver.decodedIRData.command; // A gombkód eltárolása
    Serial.print("Gombkod: ");
    Serial.println(command); // Gombkód kiírása a Serial Monitorra
    IrReceiver.resume();  // Új adat fogadása

    // LCD frissítése a megfelelő ECU adatokkal
    lcd.clear();
    lcd.setCursor(0, 0);//lcd.setCursor(oszlop,sor)

    if (command == 69) {  // Ha az "1" gombot nyomják => hőmérséklet, nedvesség, üzemanyag szint
      lcd.print("Temp |");
      lcd.setCursor(6,0);
      lcd.print("Hum  |");
      lcd.setCursor(12,0);
      lcd.print("Fuel ");
      //érékek:
      lcd.setCursor(0,1);
      lcd.print(temp);
      lcd.setCursor(5,1);
      lcd.print("|");
      lcd.setCursor(6,1);
      lcd.print(humidity);
      lcd.setCursor(11,1);
      lcd.print("|");
      lcd.setCursor(12,1);
      lcd.print(fuel_level);
    } 
    else if (command == 70) {  // Ha a "2" gombot nyomják => folytószelep, ultrahangos szenzor
      lcd.print("Throt: "); lcd.print(throttle);
      lcd.setCursor(0,1);
      lcd.print("Ultra: "); lcd.print(ultrasonic);
    } 
    else if (command == 71) {  // Ha a "3" gombot nyomják
      lcd.print("RPM: "); lcd.print(rpm);
    } 
    else {  // Ismeretlen gomb
      lcd.print("Unknown command");
    }
    }
    char key = keypad.getKey();
  
  if (key){
    lcd.clear();
    lcd.setCursor(0, 0);//lcd.setCursor(oszlop,sor)
    Serial.println(key);
    if (key == '1') {  // Ha az "1" gombot nyomják => hőmérséklet, nedvesség, üzemanyag szint
      lcd.print("Temp |");
      lcd.setCursor(6,0);
      lcd.print("Hum  |");
      lcd.setCursor(12,0);
      lcd.print("Fuel ");
      //érékek:
      lcd.setCursor(0,1);
      lcd.print(temp);
      lcd.setCursor(5,1);
      lcd.print("|");
      lcd.setCursor(6,1);
      lcd.print(humidity);
      lcd.setCursor(11,1);
      lcd.print("|");
      lcd.setCursor(12,1);
      lcd.print(fuel_level);
    } 
    else if (key == '2') {  // Ha a "2" gombot nyomják => folytószelep, ultrahangos szenzor
      lcd.print("Throt: "); lcd.print(throttle);
      lcd.setCursor(0,1);
      lcd.print("Ultra: "); lcd.print(ultrasonic);
    } 
    else if (key == '3') {  // Ha a "3" gombot nyomják
      lcd.print("RPM: "); lcd.print(rpm);
    } 
    else {  // Ismeretlen gomb
      lcd.print("Unknown command");
    }

  }

}
