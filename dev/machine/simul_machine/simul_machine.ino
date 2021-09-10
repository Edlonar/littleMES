
/**
  Arduino machine simulator
*/

#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>


//constants for pin connection
const int button_start = 6;
const int button_stop = 7;
const int led_OK = 13;
const int led_NOK = 12;
const int led_run = 10;
const int led_stop = 11;
  /* Display */
LiquidCrystal_I2C lcd3(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
/* Keypad setup */
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;
byte rowPins[KEYPAD_ROWS] = {5, 4, 3, 2};
byte colPins[KEYPAD_COLS] = {A3, A2, A1, A0};
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},
} ;

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

// Intro of the simulator
void showIntro() {
  lcd3.setCursor(2, 0);
  String message_up = "Simul Machine";
  for (byte i =0; i < message_up.length(); i++) {
    lcd3.print(message_up[i]);
    delay(50);
  }
  lcd3.setCursor(2, 1);
  String message_down = "Little MES";
  for (byte i =0; i < message_down.length(); i++) {
    lcd3.print(message_down[i]);
    delay(50);
  }
  delay(5000);
  lcd3.clear();
  lcd3.setCursor(0,0);
  lcd3.print("Choisir scenario");
}

// Setup befor the loop
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(led_OK, OUTPUT);
  pinMode(led_NOK, OUTPUT);
  pinMode(led_run, OUTPUT);
  pinMode(led_stop, OUTPUT);
  pinMode(button_start, INPUT_PULLUP);
  pinMode(button_stop, INPUT_PULLUP);
  randomSeed(analogRead(0));
  lcd3.init();
  lcd3.backlight();
  showIntro();
  lcd3.setCursor(5,1);
  lcd3.cursor();
  
}



//variables
bool choice = false;
String current = "";
String scenario = "";
String str_scenario;
long target_cycle_h = 0;
long tx_break = 0;
long tx_cycle_var = 0;
long tx_quality = 0;
bool status_running = false;
long blink_led = 0;
long blink_led_tmp = 0;
int bt_start_state = 0;
int bt_stop_state = 0;
long rand_cycle = 0;
long tmp = 0;

void updateCursor() {
  if (millis() / 250 % 2 == 0 ) {
    lcd3.cursor();
  } else {
    lcd3.noCursor();
  }
}

void choiceScenario(char key) {
  if(key == 'C') {
    //Scenario 1
    if (current == "1"){
      target_cycle_h = 10000;
      tx_break = 2;
      tx_cycle_var = 15;
      tx_quality = 5;
      scenario = "1";
      str_scenario = "Scenario 1";
      choice = true;
      lcd3.clear();
      lcd3.print(str_scenario);
      status_running = false;
      blink_led = long((3600000/target_cycle_h)) ;
    }
    else {
      lcd3.clear();
      lcd3.print("Mauvais scenario");
      delay(5000);
      choice = false;
      lcd3.clear();
      lcd3.setCursor(0,0);
      lcd3.print("Choisir scenario");
      lcd3.setCursor(5,1);
      lcd3.cursor();
      current = "";
      scenario = "";
    }
  }
  else {
    current += String(key);
    lcd3.print(key);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  updateCursor();
  bt_start_state = digitalRead(button_start);
  bt_stop_state = digitalRead(button_stop);
  char key = keypad.getKey();
  if ((key) && (choice == false)) {
    choiceScenario(key);
  }
  if (choice == true){
    if (scenario == "1") {
      if ((status_running == false) && (bt_start_state == LOW)) {
        status_running = true;
        lcd3.clear();
        lcd3.print(str_scenario);
      }
      if ((status_running == true) && (bt_stop_state == LOW)) {
        status_running = false;
        digitalWrite(led_run, LOW);
        digitalWrite(led_stop, HIGH);
        lcd3.clear();
        lcd3.print("STOP BY USER");
      }
      if (status_running == true) {
        digitalWrite(led_run, HIGH);
        digitalWrite(led_stop, LOW);
        tmp = 100 - tx_cycle_var;
        rand_cycle = random(tmp, 100);
        blink_led_tmp = (blink_led * rand_cycle)/100;
        tmp = random(0,100);
        if (tmp > tx_quality) {
          digitalWrite(led_OK, HIGH); 
          delay(blink_led_tmp);
          digitalWrite(led_OK, LOW);
          delay(blink_led_tmp); 
        }
        else {
          digitalWrite(led_NOK, HIGH); 
          delay(blink_led_tmp);
          digitalWrite(led_NOK, LOW);
          delay(blink_led_tmp); 
        }
        tmp = random(0,100);
        if (tmp < tx_break) {
          status_running = false;
          lcd3.clear();
          lcd3.print("MACHINE BREAK");
          digitalWrite(led_run, LOW);
          digitalWrite(led_stop, HIGH);
        }
      }
        
    }
  }
}
