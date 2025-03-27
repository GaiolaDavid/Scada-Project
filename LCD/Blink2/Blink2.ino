//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h> //lcd
#include <IRremote.hpp>  //távirányító, Infrared Receiver - jelfogó
#include <Keypad.h> //mini bellentyűzet

const int RECV_PIN = 2;  // Az IR szenzor DATA lábát a 2-es pinre kötöttük

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void processSerialInput();
void printLCD_master_mod(int);
void printLCD_lokal_mod(char);
void receiveData();
void IR_req_handler(int);
void data_update(String);

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


//MODBUS
int Function = 0;
int ecuNr = 3;

bool master_mod = true;

int lastEcuNr = -1;  // Inicializálás érvénytelen értékkel
int lastFunction = -1;
bool siker_kiirva = false;  // Ezzel követjük, hogy már kiírtuk-e a "siker" üzenetet
bool nincs_siker_kiirva = false;


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
  lcd.print("LCD ready");
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

    IR_req_handler(command);
    }
  // **Serial Monitor kezelése**
  // processSerialInput();

  char key = keypad.getKey();
  
  if (key == '8'){ // 8 as gomb -> master utasitasaira varok
      master_mod = true;
  } else if(key == '9') {
      master_mod = false;
  }
  if (!master_mod){
  if (key){  //ha a key !=0
    printLCD_lokal_mod(key);
  }
  }
  else{
    /*
    // Csak akkor töröljük az LCD-t, ha változás történt
    if (ecuNr != lastEcuNr || Function != lastFunction) {
      lcd.clear();
      lastEcuNr = ecuNr;  // Frissítjük az utolsó értéket
      lastFunction = Function;
    }
    printLCD_master_mod(ecuNr,Function);
  }
  */
   receiveData();
            
  }
}




//fuggvenyek 
void processSerialInput(){
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n'); 
    input.trim(); 

    if (input.startsWith("E")) {  
      int newEcu = input.substring(1).toInt();
      if (newEcu >= 1 && newEcu <= 3) {
        ecuNr = newEcu;
        Serial.print("ECU módosítva: ");
        Serial.println(ecuNr);
      } else {
        Serial.println("Hibás ECU szám!");
      }
    } 
    else if (input.startsWith("F")) {
      int newFunc = input.substring(1).toInt();
      if (newFunc >= 0 && newFunc <= 2) {  
        Function = newFunc;
        Serial.print("Funkció módosítva: ");
        Serial.println(Function);
      } else {
        Serial.println("Hibás funkció szám!");
      }
    } 
    else {
      Serial.println("Érvénytelen parancs!");
    }
  }
} // proba volt amig nem a modbus uzenetet kaptam

void printLCD_master_mod(int ecuNr){
  
  lcd.setCursor(0, 0);//lcd.setCursor(oszlop,sor)
    if (ecuNr == 1) {  // Ha az "1" gombot nyomják => hőmérséklet, nedvesség, üzemanyag szint
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
      /*if (Function == 0){

          lcd.setCursor(0,0);
          lcd.print("Temperature: ");
          lcd.setCursor(0,1);
          lcd.print(temp);
      } else if(Function == 1){
          lcd.setCursor(0,0);
          lcd.print("Humidity:");
          lcd.setCursor(0,1);
          lcd.print(humidity);
      } else if(Function == 2){
          lcd.setCursor(0,0);
          lcd.print("Fuel level:");
          lcd.setCursor(0,1);
          lcd.print(fuel_level);
      } else {  // Ismeretlen funkciókod
      lcd.setCursor(0,0);
      lcd.print("Unknown function");
    } */
    } 
    else if (ecuNr == 2) {  // Ha a "2" gombot nyomják => szelep, ultrahangos szenzor

      lcd.print("Throt: "); lcd.print(throttle);
      lcd.setCursor(0,1);
      lcd.print("Ultra: "); lcd.print(ultrasonic);

    /*if (Function == 0){
      lcd.print("Throttle: ");
      lcd.setCursor(0,1);
      lcd.print(throttle);
    } else if (Function == 1){
      lcd.setCursor(0,0);
      lcd.print("Ultrasonic: ");
      lcd.setCursor(0,1);
      lcd.print(ultrasonic);
    } else {  // Ismeretlen funkciókod
      lcd.setCursor(0,0);
      lcd.print("Unknown function");
    } 
      */
    } 
    else if (ecuNr == 3) {  // Ha a "3" gombot nyomják

      lcd.print("RPM: "); lcd.print(rpm);
    /*if (Function == 0){
      lcd.setCursor(0,0);
      lcd.print("RPM: ");
      lcd.setCursor(0,1);
      lcd.print(rpm);
    }*/
    }
    else {  // Ismeretlen funkciókod
      lcd.setCursor(0,0);
      lcd.print("Unknown command");
      lcd.setCursor(0,1);
      lcd.print("Unknown function");
    } 
}

void printLCD_lokal_mod(char key){
  lcd.clear();
    lcd.setCursor(0, 0);//lcd.setCursor(oszlop,sor)
    Serial.println(key);
    if (key == '1') {  // Ha az "1" gombot nyomják => hőmérséklet, nedvesség, üzemanyag szint
      lcd.print("Temp |");
      lcd.setCursor(6,0);
      lcd.print("  Hum |");
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

void receiveData() {
  if (Serial.available()) {
    String received = Serial.readStringUntil('\n');  // Üzenet beolvasása a végéig

    // Formátum ellenőrzése (pl. :30XX)
    if (received.length() >= 6 && received[0] == ':' && received[1] == '4' && (received[2] == '0' || received[2] == '1') && ((received[3] == '1') || (received[3] == '2') || (received[3] == '3'))  ) {

      int ecuNr_int = received[3] - '0';  // Karaktert számmá alakít
      int func_nr = received[2] - '0';

      if (func_nr == 0){ // kiíratni a képernyőre
        lcd.clear();
        printLCD_master_mod(ecuNr_int);  //a kapott paraméterek alapján kiír az LCD-re
        //Serial.println(ecuNr_int);
      } else { // adatokat frissíteni (24 bytba megkapom az összes adatot)
        data_update(received);
      }
      
      Serial.println(":50F0FFCRLF"); // Sikeres adatfeldolgozás visszajelzése

      //fgg

    } else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Hibas formatum");
      Serial.println(":500FFFCRLF"); // Sikertelen adatfeldolgozás visszajelzése
    }
  }
}

void IR_req_handler(int command){
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

void data_update(String received){
  float a,b,c,d;
  a = received[3] - '0';
  b = received[4] - '0';
  c = received[5] - '0';
  d = received[6] - '0';
  temp = a * 10 + b + (c * 0.1) + (d * 0.01);

  a = received[7] - '0';
  b = received[8] - '0';
  c = received[9] - '0';
  d = received[10] - '0';
  humidity = a * 10 + b + (c * 0.1) + (d * 0.01);

  a = received[11] - '0';
  b = received[12] - '0';
  c = received[13] - '0';
  d = received[14] - '0';
  fuel_level = a * 10 + b + (c * 0.1) + (d * 0.01);

  a = received[15] - '0';
  b = received[16] - '0';
  c = received[17] - '0';
  d = received[18] - '0';
  throttle = a * 10 + b + (c * 0.1) + (d * 0.01);

  a = received[19] - '0';
  b = received[20] - '0';
  c = received[21] - '0';
  ultrasonic = a * 10 + b + (c * 0.1);

  a = received[22] - '0';
  b = received[23] - '0';
  c = received[24] - '0';
  rpm = a * 100 + b*10 + c;
}


