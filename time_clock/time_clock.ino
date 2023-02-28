#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Time.h>
#include <TimeLib.h>
#include <Keypad.h>

/*-----------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------*/
const byte rows = 4;
const byte cols = 4;
byte rowPins[rows] = {2, 3, 4, 5};// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte colPins[cols] = {6, 7, 8, 9}; // Connect keypad COL0, COL1, COL2 and COL3 to these Arduino pins.

/*__________GLOBAR_VAR________________*/
char keys[rows][cols] = {{'1','2','3','A'},
                         {'4','5','6','B'},
                         {'7','8','9','C'},
                         {'*','0','#','D'}};   
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols); //  Create the Keypad
LiquidCrystal_I2C lcd (0x27, 20, 4);
char key; 
char EXIT = '*';
char DEL = '#';
char OK = 'A';
/*-----------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------*/

int prev_seg = second();


void setup(){
  lcd.backlight(); 
  lcd.init();
  lcd.clear();
  setTime(10,00,00,1,1,22);  
  Serial.begin(9600);
}

void loop(){
  getkey();
  if (prev_seg != second()){
    digitalClockDisplay(0, 0);
    prev_seg = second();
    Serial.println("Actualitzem segon");
  }
  if (key == 'A'){
    ;
  }
}

void digitalClockDisplay(int cX, int cY){
  printHour(hour(), minute(), second(), cX, cY, true);
  printDate(day(), month(), year(), cX-3, cY, true);
}

void printHour(int hora, int minuts, int seg,int cX, int cY, bool print_seg){
  lcd.setCursor(cX, cY);
  if (hora < 10) lcd.print('0');
  lcd.print(hora);
  lcd.print(':');
  if (minuts < 10) lcd.print('0');
  lcd.print(minuts);
  if (print_seg){
    lcd.print(':');
    if (seg < 10) lcd.print('0');
    lcd.print(seg);
  }
}


void printDate(int dia, int mes, int any, int cX, int cY, bool reduced){
  if (reduced) lcd.setCursor(cX+12, cY);
  else lcd.setCursor(cX+10, cY);
  if (dia < 10) lcd.print('0');
  lcd.print(dia);
  lcd.print('/');
  if (mes < 10) lcd.print('0');
  lcd.print(mes);
  lcd.print('/');
  if (not reduced) lcd.print(any);
  else lcd.print(any%2000);
}


/*-----------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------*/
void getkey(){
  key = kpd.getKey();
  if (key != NO_KEY){ 
    delay(100);
  } 
}
