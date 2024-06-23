#include <Keypad.h>
#include <String.h>

/*VARIABILI "GLOBALI" PROGRAMMA*/
bool is_pw_ok;
unsigned long previousMillis = 0;
const int blink_time = 500;
const int counter_light = 10;
const int num_blink = 6;
int count_blink = 0;
int ledState = LOW;

int program_start;
#define PROGRAM_START 1


/*VARIABILI FUNZIONAMENTO RELAYS*/
#define RELAY_ON LOW
#define RELAY_OFF HIGH

/*VARIABILI STATO PRIMA SERRANDA*/
#define FC1_IS_OPEN 0
#define FC1_IS_CLOSED 1
#define FC2_IS_OPEN 0
#define FC2_IS_CLOSED 1
#define GARAGE1_OPENING 0
#define GARAGE1_CLOSING 1
#define GARAGE1_OPEN 0
#define GARAGE1_CLOSED 1
#define GARAGE1_HALTED 998
int status_garage1;
int stat_FC1; //fine corsa superiore prima serranda
int stat_FC2; //fine corsa inferiore prima serranda
int FC1;
int FC2;

/*VARIABILI STATO SECONDA SERRANDA*/
#define FC3_IS_OPEN 0
#define FC3_IS_CLOSED 1
#define FC4_IS_OPEN 0
#define FC4_IS_CLOSED 1
#define GARAGE2_OPENING 0
#define GARAGE2_CLOSING 1
#define GARAGE2_OPEN 0
#define GARAGE2_CLOSED 1
#define GARAGE2_HALTED 999
int status_garage2;
int stat_FC3; //fine corsa superiore seconda serranda
int stat_FC4; //fine corsa inferiore seconda serranda
int FC3;
int FC4; 

/*VARIABILI FOTOCELLULE*/
#define FOT_1_ENABLED 1
#define FOT_2_ENABLED 1
#define FOT_1_DISABLED 0
#define FOT_2_DISABLED 0

/*VARIABILI PER FUNZIONAMENTO TASTIERA*/
const String PASSWORD = "150517";
String pw_digit;
char lastKey;
bool buttonState;
const byte ROWS = 4;
const byte COLS = 4;

//DEFINIZIONE MATRICE TASTIERA
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {26, 27, 28, 29}; //connect to the column pinouts of the keypad
byte colPins[COLS] = {22, 23, 24, 25}; //connect to the row pinouts of the keypad

//INIZIALIZZAZIONE CLASSE KEYPAD
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);
  is_pw_ok = LOW;

  /*Inizializzazione PIN LED*/
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  digitalWrite(6, HIGH);
  digitalWrite(7, LOW);

  /*Inizializzazione PIN Relay*/
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);     //relay luce
  digitalWrite(8, HIGH);   
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH);
  digitalWrite(12, HIGH);  //relay luce

  /*Inizializzazione FC*/
  pinMode(40,OUTPUT);
  pinMode(41,OUTPUT);
  pinMode(42,OUTPUT);
  pinMode(43,OUTPUT);

  program_start = PROGRAM_START;
}

void loop() {
  unsigned long currentMillis = millis();

  char customKey = customKeypad.getKey();
  if (customKey != LOW) 
    lastKey = customKey;

  keybManagement(customKey, currentMillis);

  if(program_start == PROGRAM_START){      
    stat_FC2 = digitalRead(41);
    stat_FC4 = digitalRead(43);  
    if (stat_FC2 != FC2_IS_CLOSED || stat_FC4 != FC4_IS_CLOSED) {
      while(stat_FC1 != FC1_IS_CLOSED || stat_FC3 != FC3_IS_CLOSED){ 
        stat_FC1 = digitalRead(40);
        stat_FC3 = digitalRead(42);
    
        if(stat_FC1 == FC1_IS_OPEN){ 
          digitalWrite(11, RELAY_ON); 
          status_garage1 = GARAGE1_OPENING;
        } else {
          digitalWrite(11, RELAY_OFF); 
          status_garage1 = GARAGE1_OPEN;
        }
        if(stat_FC3 == FC3_IS_OPEN){ 
        digitalWrite(9, RELAY_ON); 
        status_garage2 = GARAGE2_OPENING;
        } else {
          digitalWrite(9, RELAY_OFF); 
          status_garage2 = GARAGE2_OPEN;
        }

      }
   } 
   program_start = 0;
 }   

  relayManagement(lastKey, customKeypad, is_pw_ok);
/*
  if (currentMillis - previousMillis >= blink_time) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (digitalRead(12) == LOW) {
      digitalWrite(12,HIGH);
    } else {
      digitalWrite(12, LOW);
    }

    // set the LED with the ledState of the variable:
    //digitalWrite(6, ledState);
  }
*/
}

int relayManagement(char lastKey, Keypad customKeypad, bool is_pw_ok) {
  
  stat_FC1 = digitalRead(40);
  stat_FC2 = digitalRead(41); 
  stat_FC3 = digitalRead(42);
  stat_FC4 = digitalRead(43);  
  
  if (!is_pw_ok){
    digitalWrite(11, RELAY_OFF);
    digitalWrite(10, RELAY_OFF);
    digitalWrite(9, RELAY_OFF);
    digitalWrite(8, RELAY_OFF);
    return 100;
  }
  
  if (customKeypad.getState() == PRESSED || customKeypad.getState() == HOLD)
    buttonState = HIGH;
    stat_FC1 = digitalRead(40);
    stat_FC2 = digitalRead(41);
    stat_FC3 = digitalRead(42);
    stat_FC4 = digitalRead(43);
  if (lastKey == 'A' && buttonState == HIGH) {
    digitalWrite(10, RELAY_OFF);
    digitalWrite(11, RELAY_ON);
    status_garage1 = GARAGE1_OPENING;
  }
  else if (lastKey == 'B' && buttonState == HIGH) {
    digitalWrite(11, RELAY_OFF);
    digitalWrite(10, RELAY_ON);
    status_garage1 = GARAGE1_CLOSING;
  }
  else if (lastKey == 'C' && buttonState == HIGH) {
    digitalWrite(8, RELAY_OFF);
    digitalWrite(9, RELAY_ON);
    status_garage2 = GARAGE2_OPENING;
  }
  else if (lastKey == 'D' && buttonState == HIGH) {
    digitalWrite(9, RELAY_OFF);
    digitalWrite(8, RELAY_ON);
    status_garage2 = GARAGE2_CLOSING;
  }    
  else if (lastKey == '#' && buttonState == HIGH) {
    digitalWrite(11, RELAY_OFF);
    digitalWrite(10, RELAY_OFF);
    digitalWrite(9, RELAY_OFF);
    digitalWrite(8, RELAY_OFF);
  }      
  if(stat_FC1 == FC1_IS_CLOSED && status_garage1 == GARAGE1_OPENING) {
    digitalWrite(11, RELAY_OFF);
    digitalWrite(10, RELAY_OFF);
    buttonState = LOW;
    status_garage1 = GARAGE1_OPEN;
    return 200;
  }
  if(stat_FC2 == FC2_IS_CLOSED && status_garage1 == GARAGE1_CLOSING) {
    digitalWrite(11, RELAY_OFF);
    digitalWrite(10, RELAY_OFF);
    buttonState = LOW;
    status_garage1 = GARAGE1_CLOSED;
    return 200;
  }
  if(stat_FC3 == FC3_IS_CLOSED && status_garage2 == GARAGE2_OPENING) {
    digitalWrite(9, RELAY_OFF);
    digitalWrite(8, RELAY_OFF);
    buttonState = LOW;
    status_garage2 = GARAGE2_OPEN;
    return 200;
  }
  if(stat_FC4 == FC4_IS_CLOSED && status_garage2 == GARAGE2_CLOSING) {
    digitalWrite(9, RELAY_OFF);
    digitalWrite(8, RELAY_OFF);
    buttonState = LOW;
    status_garage2 = GARAGE2_CLOSED;
    return 200;
  }   
  if(analogRead(A0) != 0 && status_garage1 == GARAGE1_CLOSING){
    digitalWrite(11, RELAY_OFF);
    digitalWrite(10, RELAY_OFF);
    status_garage1 = GARAGE1_HALTED;
    buttonState = LOW;
  }
  if(analogRead(A1) != 0 && status_garage2 == GARAGE2_CLOSING){  
    digitalWrite(9, RELAY_OFF);
    digitalWrite(8, RELAY_OFF);
    status_garage2 = GARAGE2_HALTED;
    buttonState = LOW;
  }
  return 0;  
}

void keybManagement(char customKey, unsigned long int currentMillis) {
  if (customKey) {
    Serial.println(customKey);
    if (customKey != 'A' && customKey != 'B' && customKey != 'C' && customKey != 'D' && customKey != '*' && customKey != '#')
      pw_digit += customKey;
    Serial.println(pw_digit);
  }
  if (customKey == '#') {
    if (pw_digit == PASSWORD) {
      Serial.println("Password ok!");
      pw_digit = "";
      is_pw_ok = HIGH;
      digitalWrite(6, LOW);
      for (int i = 0; i < 5; i++) {
        digitalWrite(7, LOW);
        delay(100);
        digitalWrite(7, HIGH);
        delay(100);
      }
    }
    else {
      Serial.println("Password errata!");
      pw_digit = "";
      if (is_pw_ok == LOW) {
        for (int i = 0; i < 3; i++) {
          digitalWrite(6, LOW);
          delay(300);
          digitalWrite(6, HIGH);
          delay(300);
        }
      }
    }
  }
  if (customKey == '*') {
    pw_digit = "";
    Serial.println("Variabile pw_digit ripulita");
    is_pw_ok = LOW;
    digitalWrite(6, HIGH);
    digitalWrite(7, LOW);
    for (int i = 0; i < 5; i++) {
      digitalWrite(6, LOW);
      delay(100);
      digitalWrite(6, HIGH);
      delay(100);
    }    
  }
}
