#include <LiquidCrystal_I2C.h>  //llibreria LDC+I2C
#include <SPI.h>                //llibreria bus SPI
#include <MFRC522.h>            //llibreria especifica per MFRC522
#include <Keypad.h>
#include <Time.h>
#include <TimeLib.h>
#include <EEPROM.h>

#define RST_PIN  5      // constante para referenciar pin de reset
#define SS_PIN  53     // constante para referenciar pin de slave select

/* ==================================================== *
                         E S T A T S
 * ==================================================== */

 
/* ==================================================== *
                        C L A S S E S
 * ==================================================== */
struct Data{
  short a;
  byte mes;
  byte d;
  byte h;
  byte m;
  byte s;
};

struct Usuari{
  byte id;
  short memoria;
  Data ultimaCompra;
  Data ultimaAgafada;  
};

struct Admin{
  short memoria_1;
  short memoria_2;
};

/* ==================================================== *
                      V A R I A B L E S
 * ==================================================== */
const byte rows = 4;
const byte cols = 4;
byte rowPins[rows] = {2, 3, 4, 5};// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte colPins[cols] = {6, 7, 8, 9}; // Connect keypad COL0, COL1, COL2 and COL3 to these Arduino pins.
char keys[rows][cols] = {{'1','2','3','A'},
                         {'4','5','6','B'},
                         {'7','8','9','C'},
                         {'*','0','#','D'}};
                         
const byte maxUsuaris = 20;
byte idx;
byte usuariAct;
String error;
byte lecturaUID[4];
byte adminUID_1[4];
byte adminUID_2[4];
char codi[6];
char lecturaCodi[6];
char key;
byte contadorCodi;
                         
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols); //  Create the Keypad
LiquidCrystal_I2C lcd(0x27, 20, 4);
MFRC522 mfrc522(SS_PIN, RST_PIN); // crea objeto mfrc522 enviando pines de slave select y reset
Usuari usuaris[maxUsuaris];
Admin admin;
Data data;



/* ==================================================== *
                  I N I C I A L I T Z A C I Ó
 * ==================================================== */
void setup(){
  data.a = 2022;
  data.mes = 1;
  data.d = 1;
  data.m = 0;
  setTime(data.h, data.m, 00, data.d, data.mes, data.a);

  for (byte i = 0; i < maxUsuaris; ++i){
    usuaris[i].id = i;
    EEPROM.update(i*5, 0);
    usuaris[i].memoria = i*5;   // mem -> | saldo | uid.v1 | uid.v2 | uid.v3 | uid.v4 |
    usuaris[i].ultimaCompra = data;
    usuaris[i].ultimaAgafada = data;
  }
  contadorCodi = 0;
  admin.memoria_1 = maxUsuaris * 5;
  admin.memoria_2 = admin.memoria_1 + 4;
  assignarUID(adminUID_1, 1,2,3,4);
  assignarUID(adminUID_2, 5,6,7,8);
  for (byte i = 0; i < 4; ++i){
    EEPROM.update(admin.memoria_1 + i, adminUID_1[i]);
    EEPROM.update(admin.memoria_2 + i, adminUID_2[i]);
  }
  
  lcd.backlight();
  lcd.init();
  lcd.clear();

  resetValors();
  
  SPI.begin();        // inicializa bus SPI
  mfrc522.PCD_Init(); // inicializa modulo lector
}

/* ==================================================== *
                     M A I N    L O O P
 * ==================================================== */
void loop(){
  if (idx == 0) {menu_lectura();}
  else if (idx == 1) {menu_error();}
  else if (idx == 2) {menu_dispensar();}
  else if (idx == 3) {menu_admins();}
}




/* ==================================================== *
                      F U N C I O N S 
 * ==================================================== */

void menu_lectura(){
  key = kpd.getKey();
  if (key != NO_KEY){
    if (contadorCodi == 5){ //comprovar
      lecturaCodi[contadorCodi] = key;
      contadorCodi = 0;
      if (codisIguals(lecturaCodi, codi)){
        idx = 3;
      }
      else{
        idx = 1;
        error = "Codi Incorrecte";
        return;
      }
    }
    else {
      lecturaCodi[contadorCodi] = key;
      ++contadorCodi;
    }
  }
  if ( ! mfrc522.PICC_IsNewCardPresent())   // si no hay una tarjeta presente
    return;           // retorna al loop esperando por una tarjeta
  
  if ( ! mfrc522.PICC_ReadCardSerial())     // si no puede obtener datos de la tarjeta
    return;           // retorna al loop esperando por otra tarjeta

  for (byte i = 0; i < mfrc522.uid.size; i++){
      lecturaUID[i] = mfrc522.uid.uidByte[i]; 
  }

  bool trobat = false;
  for (int i = 0; i < maxUsuaris and not trobat; ++i){
    if (uidIguals(lecturaUID, i)){
      trobat = true;
      usuariAct = i;
      idx = 2;    // menu_dispensar();
    }
  }
  if (not trobat){
    if (uidIguals(lecturaUID, -1)){
      idx = 3;    // menu_admins();
    }
    else {
      idx = 1; // menu_error();
      error = "Tarjeta  invàlida";
    }
  }          
  mfrc522.PICC_HaltA();     // detiene comunicacion con tarjeta 
}

void menu_error(){
  lcd.setCursor(1,0);
  lcd.print(error);
  delay(2000);
  resetValors();
}

void menu_dispensar(){
  if (validUsuari(usuariAct)){
    byte saldo = EEPROM.read(usuaris[usuariAct].memoria);
    if (saldo < 1){
      idx = 1;
      error = "No tens saldo";
    }
    else {
      --saldo;
      EEPROM.update(usuaris[usuariAct].memoria, saldo); //EEPROM.write(address, value) EEPROM.read(address) EEPROM.update(address, value)
      idx = 1; //menu_error();
      error = "Dispensant...";
    }
  }
  else {
    idx = 1;  // menu_error();
    error = "Usuari invalid";
  }
}

void menu_admins(){
  ;
}

bool uidIguals(byte lectura[], byte i){
  if (i == -1){ // admin
    for (byte i = 0; i < 4; ++i){
      if (lectura[i] != EEPROM.read(admin.memoria_1+i)){
        return false;
      }
    }
    return true;
    for (byte i = 0; i < 4; ++i){
      if (lectura[i] != EEPROM.read(admin.memoria_2+i)){
        return false;
      }
    }
    return true;
  }
  else {
    short memoria = usuaris[i].memoria;
    for (byte i = 0; i < 4; ++i){
      if (lectura[i] != EEPROM.read(memoria+1+i)){
        return false;
      }
    }
    return true;
  }
}

bool validUsuari(byte id){
  return (id >= 0 and id < maxUsuaris);
}

void assignarUID(byte uid[], byte a, byte b, byte c, byte d){
  uid[0] = a; uid[1] = b;
  uid[2] = c; uid[3] = d;
}

bool codisIguals(char lecturaCodi[], char codi[]){
  for (byte i = 0; i < 6; ++i){
    if (lecturaCodi[i] != codi[i]){
      return false; 
    }
  }
  return true;
}

void resetValors(){
  idx = 0;
  for (byte i = 0; i < 4; ++i){
    lecturaUID[i] = 0;  
  }
  error = "Error inesperat";
  usuariAct = -1;
}



 
