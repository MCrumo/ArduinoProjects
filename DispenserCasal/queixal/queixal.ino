/*___________HEATHERS_________________*/
#include <Key.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Time.h>
#include <TimeLib.h>
#include <Servo.h> /*______________SERVO_________________*/

/*_________________PINS_______________*/
const byte pinServo = 10;
const byte pinSensor = 11;
const short pulsMin = 800; /*______________SERVO_________________*/
const short pulsMax = 2200; /*______________SERVO_________________*/
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
Servo motor; /*______________SERVO_________________*/

const String PROD = "BIRRES"; // producte a la nevera
const short MAX_USERS = 20; //maxim d'usuaris a l'aplicatiu
const short FIRST_ID = 1001;
short total_prod = -1; // total del producte
short admin_id = 9999;
char key; 
short idx = 0;
char EXIT = '*';
char DEL = '#';
char OK = 'A';
short id_idx = 0;
short pass_idx = 0;
short quant = 0;
short act_user_idx = -1;
String user_id = "****";
String user_pass = "____";
String prove_pass = "....";
String new_prod = "_";
bool first_print = true;
int prev_seg = second();
String set_h = "__";
String set_min = "__";
String set_dia = "__";
String set_mes = "__";
String set_any = "____";
int act_h = 0;
int act_m = 0;
int act_d = 1;
int act_mes = 1;
int act_a = 2022;
int light = 1023;

struct date {
  int h;
  int m;
  int d;
  int mes;
  int a;
};

struct user {
  bool ini_pass;    //is password initialized?
  short id;         //user id
  String password;  //user password
  short prod;       //total quantity of product of that user (ex: if prod = 5, hi has 5 beers)
  date last_mod;    //last day user did a modification
  
};

user USERS[MAX_USERS];
user ADMIN;
//Canviar password usuaris
//Canviar total birres usuaris
//Mirar el total de birres de cada usuari
//SI ES FALLA 3 COPS LA CONTRA D'ADMIN ELS BLOQUEJA EL DISPOSITIU

/*_____________CONFIG__________*/
void setup(){
  motor.attach(pinServo, pulsMin, pulsMax); /*______________SERVO_________________*/
  lcd.backlight(); 
  lcd.init();
  lcd.clear();
  set_users();
  setTime(act_h, act_m, 00, act_d, act_mes, act_a);  
  /*___DEBUG___*/
  Serial.begin(9600);
  Serial.println("Setup complete");
  /*___DEBUG___*/
}

/*____________MAIN_LOOP________*/
void loop(){
  getkey();
  if (idx == 0) {menu_welcome(); action_0();}         //menu: principal/inicial
  else if (idx == 1) {menu_info(); action_1();}       //menu: info i canviar hora
  else if (idx == 2) {menu_prod(); action_2();}       //menu: canviar la quantitat de producte -> id admin
  else if (idx == 3) {menu_prod2(); action_3();}      //menu: canviar la quantitat de producte -> pass admin
  else if (idx == 4) {menu_prod3(); action_4();}      //menu: canviar la quantitat de producte -> definitiu 
  else if (idx == 5) {menu_agafar(); action_5();}     //menu: agafar producte -> id usuari
  else if (idx == 6) {menu_agafar2(); action_6();}    //menu: agafar producte -> pass usuari
  else if (idx == 7) {menu_agafar3(); action_7();}    //menu: agafar producte -> definitiu
  else if (idx == 8) {menu_admins(); action_8();}     //menu: edicio dels admins -> id admin
  else if (idx == 9) {menu_admins2(); action_9();}    //menu: edicio dels admins -> pass admin
  else if (idx == 10) {menu_admins3(); action_10();}  //menu: edicio dels admins -> triar edicio
  else if (idx == 11) {menu_change(); action_11();}   //menu: canviar/mirar password d'un usuari -> id usuari 
  else if (idx == 12) {menu_change2(); action_12();}  //menu: canviar/mirar password d'un usuari -> set new password
  else if (idx == 13) {menu_look(); action_13();}     //menu: canviar/mirar producte d'un usuari -> id usuari 
  else if (idx == 14) {menu_look2(); action_14();}    //menu: canviar/mirar producte d'un usuari -> set new quantity
  else if (idx == 15) {menu_vis(); action_15();}      //menu: visualitzar tota la info dels usuaris
  else if (idx == 16) {menu_info2(); action_16();}      //menu: info pels consumidors
  else if (idx == 17) {menu_hora(); action_17();}       //menu: canviar hora -> pass admin 
  else if (idx == 18) {menu_hora2(); action_18();}      //menu: canviar hora -> definitiu
}
/*menu: principal/inicial*/
void menu_welcome(){
  if (prev_seg != second()){
    digitalClockDisplay(0, 0);
    prev_seg = second();
  }
  if (first_print){
    print_lcd("","Birres: | "+String(total_prod)+ " |","AGAFAR "+PROD,"admin","[A]","[B]","[C]","[D]");
    first_print = false;
  }
  
}
void action_0(){
  if (key == 'A') {lcd.clear(); idx = 1;}
  if (key == 'B') {lcd.clear(); idx = 2;}
  if (key == 'C') {lcd.clear(); idx = 5;}
  if (key == 'D') {lcd.clear(); idx = 8;}
}
/*menu: info pels consumidors*/
void menu_info() {
  print_lcd("Canvia Hora:","Info:","EXIT","made by Miquel Romeo","[A]","[B]","["+String(EXIT)+"]","");
}
void action_1(){
  if (key == EXIT) {undo(0);}
  else if (key == 'A') {lcd.clear(); idx = 17; user_pass = "____"; pass_idx = 0;} //canviar hora
  else if (key == 'B') {lcd.clear(); idx = 16;} //info
}
/*menu: canviar la quantitat de producte -> id admin*/
void menu_prod(){
  if (not ADMIN.ini_pass) print_lcd("INTRO ADMIN ID:",user_id,"DELETE","EXIT",  "","","["+String(DEL)+"]","["+String(EXIT)+"]");
  else{
    idx = 3;
    user_pass = "____";
    pass_idx = 0;
  }
}
void action_2(){
  if (key == EXIT) {undo(0);}
  else {
    if (not ADMIN.ini_pass) prove_user_id(true, 3, 0);
  }
}
/*menu: canviar la quantitat de producte -> pass admin*/
void menu_prod2(){
  if (ADMIN.ini_pass) print_lcd("INTRO ADMIN PASSWORD",user_pass,"DELETE","EXIT",  "","","["+String(DEL)+"]","["+String(EXIT)+"]");
  else { //password not initialized
    print_lcd("SET ADMIN PASSWORD:",user_pass,"DELETE","EXIT",  "","["+String(OK)+"]","["+String(DEL)+"]","["+String(EXIT)+"]");
  }
}
void action_3(){
  if (key == EXIT) {undo(0);} 
  if (ADMIN.ini_pass) {
    prove_user_pass(ADMIN, 4);
  }
  else { //password not initialized
    set_user_pass(ADMIN, 0);
  }
}
/*menu: canviar la quantitat de producte -> definitiu*/
void menu_prod3(){ 
  print_lcd("Total: "+String(total_prod)+" "+PROD,"Nou total: ","DELETE","EXIT",   "","["+String(OK)+"]","["+String(DEL)+"]","["+String(EXIT)+"]");
  lcd.setCursor(11,1);
  lcd.print(new_prod);
}
void action_4(){
  if (key == EXIT) {undo(0);} 
  else if (is_num(key)){
    if (new_prod == "_" and key != '0')new_prod = String(key);
    else new_prod = new_prod + String(key);
  }
  else if (key == DEL and new_prod != "_"){
    new_prod = "_";
    lcd.clear();
  }
  else if (key == OK) {
    if (new_prod != "" and new_prod != "_") total_prod = new_prod.toInt();
    error_exit("new_prod", 0);
  }
}
/*menu: agafar producte -> id usuari*/
void menu_agafar(){
  if (total_prod < 1) {error_exit("no_prod", 0);}
  else print_lcd("INTRO YOUR ID:",user_id,"DELETE","EXIT",  "","","["+String(DEL)+"]","["+String(EXIT)+"]");
}
void action_5(){
  if (key == EXIT) {undo(0);}
  else if (total_prod > 0){
    prove_user_id(false, 6, 0);
  }
}
/*menu: agafar producte -> password usuari*/
void menu_agafar2(){
  if ( act_user_idx == -1 or get_prod(user_of(act_user_idx)) > 0){
    if (is_pass_init(user_of(act_user_idx))){ 
      print_lcd("INTRO YOUR PASSWORD",user_pass,"DELETE","EXIT",  "","","["+String(DEL)+"]","["+String(EXIT)+"]");
    }
    else { //password not initialized
      print_lcd("SET YOUR PASSWORD:",user_pass,"DELETE","EXIT",  "","["+String(OK)+"]","["+String(DEL)+"]","["+String(EXIT)+"]");
    }
  }
}
void action_6(){
  if (key == EXIT) {undo(0);} 
  if (act_user_idx > -1 and get_prod(user_of(act_user_idx)) < 1){
    error_exit("no_saldo", 0);
  }
  else {
    if (is_pass_init(user_of(act_user_idx))) {
      prove_user_pass(user_of(act_user_idx), 7);
    }
    else { //password not initialized
      set_user_pass(user_of(act_user_idx), 0);
    }
  }
}
/*menu: agafar producte -> definitiu*/
void menu_agafar3(){
  print_lcd("Saldo "+PROD+": "+String(get_prod(user_of(act_user_idx))),"Vols agafar:","DELETE","EXIT","","["+String(OK)+"]","["+String(DEL)+"]","["+String(EXIT)+"]");
  lcd.setCursor(13,1);
  lcd.print(new_prod);
}
void action_7(){
  if (key == EXIT) {undo(0);} 
  else if (is_num(key)){
    if (new_prod == "_") new_prod = String(key);
    else new_prod = new_prod + String(key);
  }
  else if (key == DEL and new_prod != "_"){
    new_prod = "_";
    lcd.clear();
  }
  else if (key == OK and act_user_idx > -1) {
    if (new_prod != "_" and new_prod != "" and new_prod[0] != '0') { //valid number
      quant = new_prod.toInt();
      if (quant > get_prod(user_of(act_user_idx))) error_exit("no_saldo", 0);
      else if (quant > total_prod) error_exit("too_many", 7);
      else {
        obrir_nevera(quant);
      }
    }
    else error_exit("not_char", 0);
  }
  else if (act_user_idx < 0) obrir_nevera(0);
}
/*menu: admins -> admin id*/
void menu_admins(){
  if (not ADMIN.ini_pass) print_lcd("INTRO ADMIN ID:",user_id,"DELETE","EXIT",  "","","["+String(DEL)+"]","["+String(EXIT)+"]");
  else {
    idx = 9;
    user_pass = "____";
    pass_idx = 0;
  }
  
}
void action_8(){
  if (key == EXIT) {undo(0);}
  else {
    if (not ADMIN.ini_pass) prove_user_id(true, 9, 0);
  }
}
/*menu: admins -> admin password*/
void menu_admins2(){
  if (ADMIN.ini_pass) print_lcd("INTRO ADMIN PASSWORD",user_pass,"DELETE","EXIT",  "","","["+String(DEL)+"]","["+String(EXIT)+"]");
  else { //password not initialized
    print_lcd("SET ADMIN PASSWORD:",user_pass,"DELETE","EXIT",  "","["+String(OK)+"]","["+String(DEL)+"]","["+String(EXIT)+"]");
  }
}
void action_9(){
  if (key == EXIT) {undo(0);} 
  if (ADMIN.ini_pass) {
    prove_user_pass(ADMIN, 10);
  }
  else { //password not initialized
    set_user_pass(ADMIN, 0);
  }
}
/*menu: admins -> gestionar info amb nou menu*/
void menu_admins3(){
  print_lcd("Modificar PIN","Modificar SALDO","Veure USERS","EXIT",  "[A]","[B]","[C]","["+String(EXIT)+"]");
}
void action_10(){
  if (key == EXIT) {undo(0);} 
  else if (key == 'A') {lcd.clear(); idx = 11;}
  else if (key == 'B') {lcd.clear(); idx = 13;}
  else if (key == 'C') {lcd.clear(); idx = 15;}
}
/*menu: canviar password usuari -> intro id*/
void menu_change(){
  print_lcd("INTRO USER ID:",user_id,"DELETE","EXIT",  "","","["+String(DEL)+"]","["+String(EXIT)+"]");
}
void action_11(){
  if (key == EXIT) {undo(10);}
  else {
    prove_user_id(false, 12, 10);
  }
}
/*menu: canviar o veure passwrod usuari -> nou password*/
void menu_change2(){
  print_lcd("id:"+String(USERS[act_user_idx].id)+" pin:"+String(USERS[act_user_idx].password),user_pass,"DELETE","EXIT","","["+String(OK)+"]","["+String(DEL)+"]","["+String(EXIT)+"]");
}
void action_12(){
  if (key == EXIT) {undo(10);}
  else set_user_pass(user_of(act_user_idx), 10);
}
/*menu: canviar birres usuari -> intro id*/
void menu_look(){
  print_lcd("INTRO USER ID:",user_id,"DELETE","EXIT",  "","","["+String(DEL)+"]","["+String(EXIT)+"]");
}
void action_13(){
  if (key == EXIT) {undo(10);}
  else {
    prove_user_id(false, 14, 10);
  }
}
/*menu: veure o canviar birres usuari -> nou password*/
void menu_look2(){
  print_lcd(String(USERS[act_user_idx].id)+" te: "+String(USERS[act_user_idx].prod)+" "+PROD,"Nova cantitat:","DELETE","EXIT","","["+String(OK)+"]","["+String(DEL)+"]","["+String(EXIT)+"]");
  lcd.setCursor(14,1);
  lcd.print(new_prod);
}
void action_14(){
  if (key == EXIT) {undo(10);} 
  else if (is_num(key)){
    if (new_prod == "_") new_prod = String(key);
    else new_prod = new_prod + String(key);
  }
  else if (key == DEL and new_prod != "_"){
    new_prod = "_";
    lcd.clear();
  }
  else if (key == OK) {
    if (new_prod != "_" and new_prod != "" and new_prod[0] != '0') { //valid number
      if (act_user_idx >= 0 and act_user_idx < MAX_USERS){
        USERS[act_user_idx].prod = new_prod.toInt();
        error_exit("new_prod", 10);
      }
      else if (act_user_idx == -1){
       ADMIN.prod = new_prod.toInt();
       error_exit("new_prod", 10);
      }
      else{
        error_exit("id", 10);
      }
    }
    else error_exit("not_char", 10);
  }
}
void menu_vis(){
  short i = -1;
  while (i < MAX_USERS){
    getkey();
    print_lcd("id: "+String(USERS[i].id)+"  pass: "+USERS[i].password,"Saldo "+PROD+": "+String(USERS[i].prod),"Last: ","<-[#] [D]->  Exit["+String(EXIT)+"]",    "","","","");
    printHour(USERS[i].last_mod.h, USERS[i].last_mod.m, 0, 6, 2, false);
    printDate(USERS[i].last_mod.d, USERS[i].last_mod.mes, USERS[i].last_mod.a, 0, 2, true);
    if (key == 'D' and i < MAX_USERS-1) {++i; lcd.clear();}
    else if (key == DEL and i > -1) {--i; lcd.clear();}
    else if (key == EXIT) break; 
  }
}
void action_15(){
  undo(10);
}
/*menu: info pels consumidors*/
void menu_info2(){
  lcd.setCursor(0,0);
  lcd.print("Menu d'informacio");
} 
void action_16(){
  if (key == EXIT) {undo(0);} 
  Serial.println(key);
  Serial.println(idx);
}      
/*menu: canviar hora -> pass admin*/
void menu_hora(){
  if (ADMIN.ini_pass) print_lcd("INTRO ADMIN PASSWORD",user_pass,"DELETE","EXIT",  "","","["+String(DEL)+"]","["+String(EXIT)+"]");
  else { //password not initialized
    error_exit("no_ini_pass",0);
  }
}
void action_17(){
  if (key == EXIT) {undo(0);} 
  if (not ADMIN.ini_pass) error_exit("unsep_error", 0);
  else prove_user_pass(ADMIN, 18);
  Serial.println(key);
}
/*menu: canviar hora -> definitiu*/
void menu_hora2(){                                                      
  print_lcd("Hora: "+set_h+"  min: "+set_min,"Dia: "+set_dia+"  Mes: "+set_mes,"Any: "+set_any,"OK["+String(OK)+"]  EXIT["+String(EXIT)+"]",  "","","","");
} 
void action_18(){
  if (key == EXIT) {undo(0);} 
  set_hour_date();
}




/*________________________________________*/
/*_____________AUX_FUNCTIONS______________*/
/*________________________________________*/
/*Prove if an id is valid, if valid goes to next_menu*/
void prove_user_id(bool only_admin, const short& next_menu, const short& next_idx){
  if (id_idx == 4){
    delay(500);
    id_idx = 0;
    if (valid_id(user_id.toInt(), only_admin)){ 
      idx = next_menu;
      if (user_id.toInt() != admin_id) act_user_idx = user_id.toInt()-FIRST_ID; //no admin user
      else act_user_idx = -1; //admin user
      user_id = "****";
      user_pass = "____";
      pass_idx = 0;
      lcd.clear();
    }
    else {
      error_exit("id", next_idx);
    }
  }
  else if (id_idx < 4 or key == DEL) {
    if (key == DEL and id_idx > 0){
      --id_idx;
      user_id[id_idx] = '*';
    }
    else if (is_num(key)) {
      user_id[id_idx] = key;
      ++id_idx;
    }
  }
}
/*Set the user password; ini_pass has to be false*/
void set_user_pass(user& USER, const short& next_idx){
    if (pass_idx == 4){
        if (key == OK){
            pass_idx = 0;
            USER.password = user_pass;
            USER.ini_pass = true;
            error_exit("pass_set", next_idx);
        }
    }
    if (pass_idx < 4 or key == DEL){
        if (key == DEL and pass_idx > 0){
            --pass_idx;
            user_pass[pass_idx] = '_';
        }
        else if (is_char(key)){
            user_pass[pass_idx] = key;
            ++pass_idx;
        }
    }
}
/*Prove if the password if correct; ini_pass has to be true*/
void prove_user_pass(user& USER, short next_menu){
  if (pass_idx == 4){
    pass_idx = 0;
    if (prove_pass == USER.password) { //password correct
        idx = next_menu;
        user_pass = "____";
        user_id = "****";
        lcd.clear();
    }
    else { //password incorrect
        error_exit("pass_inc", 0);
    }
  }
  if (pass_idx < 4 or key == DEL){
    if (key == DEL and pass_idx > 0){
      --pass_idx;
      user_pass[pass_idx] = '_';
    }
    else if (is_char(key)){
        user_pass[pass_idx] = '*';
        prove_pass[pass_idx] = key;
        ++pass_idx;
    }
  }
}
/*Quets the total prod of a User*/
short get_prod(const user& USER){
  return USER.prod;
}
/*Gets the key of the keyPad*/
void getkey(){
  key = kpd.getKey();
  if (key != NO_KEY){ 
    delay(100);
  } 
}
/*Given a idx returns its user*/
user& user_of(const short& act_user_idx){
    if (act_user_idx >= 0 and act_user_idx < MAX_USERS) return USERS[act_user_idx];
    else if (act_user_idx == -1) return ADMIN;
    else error_exit("error", 0);
}
/*Return if USER password is initialized*/
bool is_pass_init(const user& USER){
    return USER.ini_pass;
}
/*Returns if key is a character*/
bool is_char(const char& key){
    return (key=='1' or key=='2' or key=='3' or key=='4' or key=='5' or key=='6' or key=='7' or 
            key=='8' or key=='9' or key=='0' or key=='A' or key=='B' or key=='C' or key=='D');
}
/*Returs if key is a number*/
bool is_num(const char& key){
    return (key=='1' or key=='2' or key=='3' or key=='4' or key=='5' or 
            key=='6' or key=='7' or key=='8' or key=='9' or key=='0');
}
/*Returs if and user id is valid*/
bool valid_id(const short& id, bool only_admin){
    if (id == admin_id or (id >= FIRST_ID and id < FIRST_ID + MAX_USERS and not only_admin)){
        return true;
    }
    else return false;
}
/*Exit to welcome_menu whit a lcd message*/
void error_exit(const String& pid, const short& next_idx){
  if (pid == "id"){
    lcd.clear();
    lcd.setCursor(3,1);
    lcd.print("Error id usuari");
    delay(2000);
  }
  else if (pid == "pass_set"){
    lcd.clear();
    lcd.setCursor(2,1);
    lcd.print("> Password set !");
    delay(2000);
  }
  else if (pid == "new_prod"){
    lcd.clear();
    lcd.setCursor(2,1);
    lcd.print(">  New total set !");
    delay(2000);
  }
  else if (pid == "pass_inc"){
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("INCORRECT password");
    delay(2000);
  }
  else if (pid == "too_many"){
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("No hi ha suficients");
    lcd.setCursor(1,2);
    lcd.print("Queden "+String(total_prod)+" "+PROD);
    delay(4000);
  }
  else if (pid == "not_char"){
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("INVALID number !");
    delay(2000);
  }
  else if (pid == "no_saldo"){
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("No tens tant saldo!");
    lcd.setCursor(0,2);
    lcd.print("Actualment tens "+String(USERS[act_user_idx].prod));
    lcd.setCursor(0,3);
    lcd.print(PROD);
    delay(4000);
  }
  else if (pid == "agafar"){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Pots agafar "+String(quant)+" "+PROD);
    lcd.setCursor(0,2);
    lcd.print("Actualment tens: "+String(USERS[act_user_idx].prod));
  }
  else if (pid == "no_prod"){
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("No queden "+PROD);
    delay(2000);
  }
  else if (pid == "no_ini_pass"){
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("ERROR passwrod no");
    lcd.setCursor(0,2);
    lcd.print("inicialitzat");
    delay(2000);
  }
  else if (pid == "inc_time"){
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("Incorrect time");
    delay(2000);
  }
  else if (pid == "time_set"){
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("Time set !");
    delay(2000);
  }
  else if (pid == ""){
    ;
  }
  else {
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("> Unespected error !");
    delay(2000);
  }
  if (pid != "agafar") undo(next_idx);
}
/*Reset global var. to its initial value*/
void undo(const short& next_idx){
  idx = next_idx;
  id_idx = 0; 
  user_id = "****"; 
  pass_idx = 0; 
  user_pass = "____"; 
  new_prod = "_";
  prove_pass = "....";
  act_user_idx = -1;
  quant = 0;
  set_h = "__";
  set_min = "__";
  set_dia = "__";
  set_mes = "__";
  set_any = "____";
  first_print = true;
  light = 1023;
  lcd.clear();
}
/*Print all rows of lcd*/
void print_lcd(String a, String b, String c, String d, String a1, String b1, String c1, String d1){
  if (a != ""){
    lcd.setCursor(0,0);
    lcd.print(a);
  }
  if (a1 != ""){
    lcd.setCursor(17,0);
    lcd.print(a1);
  }
  if (b != ""){
    lcd.setCursor(0,1);
    lcd.print(b);
  }
  if (b1 != ""){
    lcd.setCursor(17,1);
    lcd.print(b1);
  }
  if (c != ""){
    lcd.setCursor(0,2);
    lcd.print(c);
  }
  if (c1 != ""){
    lcd.setCursor(17,2);
    lcd.print(c1);
  }
  if (d != ""){
    lcd.setCursor(0,3);
    lcd.print(d);
  }
  if (d1 != ""){
    lcd.setCursor(17,3);
    lcd.print(d1);
  }
}
/*print hour and date to the glcd*/
void digitalClockDisplay(int cX, int cY){
  printHour(hour(), minute(), second(), cX, cY, true);
  printDate(day(), month(), year(), cX-3, cY, true);
}
/*print hour to the glcd*/
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
/*print date to the glcd*/
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
/*Set new date*/
void set_hour_date(){
  if (is_num(key)){
    if (set_h[0] == '_') set_h[0] = key;
    else if (set_h[1] == '_') set_h[1] = key;
    else {
      if (set_min[0] == '_') set_min[0] = key;
      else if (set_min[1] == '_') set_min[1] = key;
      else {
        if (set_dia[0] == '_') set_dia[0] = key;
        else if (set_dia[1] == '_') set_dia[1] = key;
        else {
          if (set_mes[0] == '_') set_mes[0] = key;
          else if (set_mes[1] == '_') set_mes[1] = key;
          else {
            if (set_any[0] == '_') set_any[0] = key;
            else if (set_any[1] == '_') set_any[1] = key;
            else if (set_any[2] == '_') set_any[2] = key;
            else if (set_any[3] == '_') set_any[3] = key;
            else { //all date and hour set
              if (key == OK){
                int h = set_h.toInt();
                int mi = set_min.toInt();
                int d = set_dia.toInt();
                int me = set_mes.toInt();
                int a = set_any.toInt();
                if (h < 24 and mi < 59 and d < 32 and me < 13 and a > 2021 and a < 3000){
                  setTime(h,mi,00,d,me,a);
                  error_exit("time_set", 0);
                }
                else {
                  error_exit("inc_time", 0);
                }
              }
            }
          }
        }
      }
    }
  }
}
/*Set initial USERS data on void setup()*/
void set_users(){
  for (int i = 0; i < MAX_USERS; ++i){
    USERS[i].id = FIRST_ID+i;
    USERS[i].prod = 0;
    USERS[i].ini_pass = false;
    USERS[i].password = "****";
    USERS[i].last_mod.h = act_h;
    USERS[i].last_mod.m = act_m;
    USERS[i].last_mod.d = act_d;
    USERS[i].last_mod.mes = act_mes;
    USERS[i].last_mod.a = act_a;
  }
  ADMIN.ini_pass = false;
  ADMIN.id = admin_id;
  ADMIN.last_mod.h = act_h;
  ADMIN.last_mod.m = act_m;
  ADMIN.last_mod.d = act_d;
  ADMIN.last_mod.mes = act_mes;
  ADMIN.last_mod.a = act_a;
}

void obrir_nevera(short quant){
  short i = act_user_idx;
  if (i > -1){
    USERS[i].prod -= quant;
    USERS[i].last_mod.h = hour();
    USERS[i].last_mod.m = minute();
    USERS[i].last_mod.d = day();
    USERS[i].last_mod.mes = month();
    USERS[i].last_mod.a = year();
  }
  total_prod -= quant;
  //Obrim nevera
  motor.write(0);
  error_exit("agafar", 0);
  //Nerevra es tanca ->
  Serial.println(light);
  while (light > 50){
    Serial.println(light);
    light = analogRead(pinSensor);
    delay(100);
  }
  delay(1000);
  motor.write(180);
  undo(0);
}
