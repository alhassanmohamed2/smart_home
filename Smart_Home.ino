#include "DHT.h"
#include <Servo.h>
#include <Keypad.h>
#include <EEPROM.h>

#define DHTTYPE DHT11
#define DHTPIN A2
#define sound_sensor A0
#define fire_sensor A1
#define buzzer 10
#define relay1 13
#define relay2 12
#define servo_motor 9
#define fan 11
DHT dht(DHTPIN, DHTTYPE);
Servo myservo;

String readStringFromEEPROM(int);
void writeStringToEEPROM(int , const String&);


bool password_true = false;
int clap = 0;
long detection_range_start = 0;
long detection_range = 0;

bool temp_change;
int temp_limit;
String temp_get;
int pos = 0;
int lamps = 0;
int i = 0;

const int ROW_NUM = 4;
const int COLUMN_NUM = 3;
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}

};
byte pin_rows[ROW_NUM] = {2, 3, 4, 5};
byte pin_column[COLUMN_NUM] = {6, 7, 8};
Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );
String password;
String input_password;
int change_pass = 0;
String new_pass;
String check_pass;

void setup() {
  temp_limit = EEPROM.read(0);
  password = readStringFromEEPROM(1);
  pinMode(DHTPIN, INPUT);
  pinMode(sound_sensor, INPUT);
  pinMode(fire_sensor, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(servo_motor, OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  myservo.attach(servo_motor);
  myservo.write(30);
  dht.begin();
  input_password.reserve(32);

}

void loop() {
  key_pad();
  temp();
  sound();
  fire();
}


void fire() {
  int fireSensor = digitalRead(fire_sensor);
  if (fireSensor == 0) {
    tone(buzzer, 250);
    delay(1000);
    noTone(buzzer);
    delay(1000);
  }
}


void sound() {
  int status_sensor = digitalRead(sound_sensor);
  if (status_sensor == 0)
  {
    if (clap == 0)
    {
      detection_range_start = detection_range = millis();
      clap++;
    }
    else if (clap > 0 && millis() - detection_range >= 50)
    {
      detection_range = millis();
      clap++;
    }
  }
  if (millis() - detection_range_start >= 400)
  {
    if (clap == 2)
    {
      if (!lamps)
      {
        digitalWrite(relay1, HIGH);
        lamps += 1;
      }
      else if (lamps > 0 && lamps < 2)
      {
        digitalWrite(relay2, HIGH);
        digitalWrite(relay1, LOW);
        lamps += 1;
      } else if (lamps > 0 && lamps < 3)
      {
        digitalWrite(relay1, HIGH);
        digitalWrite(relay2, HIGH);

        lamps += 1;
      } else if (lamps == 3) {
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        lamps = 0;
      }
    }
    clap = 0;
  }
}

void temp() {
  float t = dht.readTemperature();
  if (t > temp_limit && t <= temp_limit + 2) {
    analogWrite(fan, 50);
  }
  else if (t >= temp_limit + 2 && t <= temp_limit + 4) {
    analogWrite(fan, 100);
  }
  else if (t >= temp_limit + 4 && t <= temp_limit + 6) {
    analogWrite(fan, 150);
  }
  else if (t >= temp_limit + 6 && t <= temp_limit + 8) {
    analogWrite(fan, 200);
  }
  else if (t >= temp_limit + 8) {
analogWrite(fan, 255);
  }
  else if (t < temp_limit) {
    analogWrite(fan, 0);
  }
 
}


void key_pad() {
  char key = keypad.getKey();
  if (key) {
    tone(buzzer, 350);
    delay(500);
    noTone(buzzer);
     if (key == '*') {
    change_pass += 1;
    temp_get = "" ;
    temp_change = true;

  }
    if (change_pass == 1) {
      update_temp(key);
    }
     else if (change_pass == 2) {
      temp_change = false;
      change_pass_fun(key);
    }

    else if (key == '#' && change_pass == 0) {
      if (password == input_password) {
        input_password = "";

        for (pos = 30; pos <= 140; pos += 1) {
          myservo.write(pos);
          delay(15);
        }
        delay(3000);
        for (pos = 140; pos >= 30; pos -= 1) {
          myservo.write(pos);
          delay(15);
        }


      } else {
        alert_sound();
      }

      input_password = "";
    } else if (key != '#' || key != '*' && change_pass == 0) {
      input_password += key;
    }
  }
}


void update_temp(char key) {
   if (temp_change && key) {
    if (key == '#') {
      change_pass = 0;
      if(temp_get.toInt() == 0 || temp_get.toInt() > 100){
        temp_limit = temp_get.toInt();
        temp_change = false;
      
        }else{
          EEPROM.update(0, temp_get.toInt());
      temp_limit = EEPROM.read(0);
      temp_change = false;
      input_password = "";
      success_sound();
          }
      
    }
    else if (key != '#' && key != '*') {
      temp_get += key;
    }
  }
}


void change_pass_fun(char key) {

  if(!password_true){
   if (key == '#') {
    change_pass = 0;
    if (check_pass == password){
    password_true = true;
    check_pass = "";
     success_sound();
    }else {
      check_pass = "";
      alert_sound();
        }
  } else if (key != '#' && key != '*') {
    check_pass += key;
  }
    }else {
  if (key == '#' && new_pass != "") {
    writeStringToEEPROM(1, new_pass);
    password = new_pass;
    change_pass = 0;
    new_pass = "";
    password_true = false;
    success_sound();
  } else if (key != '#' && key != '*') {
    new_pass += key;
  }
    }

}

void alert_sound(){
  tone(buzzer, 350);
        delay(2000);
        noTone(buzzer);
  }
void success_sound(){
  tone(buzzer, 550);
     delay(500);
     tone(buzzer, 650);
     delay(500);
     noTone(buzzer);
  }
// String Write & Read to & from EEPROM
void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.update(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.update(addrOffset + 1 + i, strToWrite[i]);
  }
}
String readStringFromEEPROM(int addrOffset)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0';
  return String(data);
}
