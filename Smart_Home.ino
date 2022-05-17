#include "DHT.h"
#include <Servo.h>
#include <Keypad.h>

#define DHTTYPE DHT11
#define DHTPIN A2
#define sound_sensor A0
#define fire_sensor A1
#define buzzer 10
#define relay1 11
#define relay2 12
#define servo_motor 9
#define fan 13 
DHT dht(DHTPIN, DHTTYPE);
Servo myservo;

int clap = 0;
long detection_range_start = 0;
long detection_range = 0;


bool on = false;
int pos = 0;
int lamps = 0;
int i = 0;

const int ROW_NUM = 4;
const int COLUMN_NUM = 3;
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*','0','#'}

};
byte pin_rows[ROW_NUM] = {2,3,4,5};
byte pin_column[COLUMN_NUM] = {6,7,8};
Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );
const String password = "1237";
String input_password;

void setup() {
  
  pinMode(DHTPIN, INPUT);
  pinMode(sound_sensor, INPUT);
  pinMode(fire_sensor, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(servo_motor, OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  myservo.attach(servo_motor);
  myservo.write(0);
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
else if (clap > 0 && millis()-detection_range >= 50)
{
detection_range = millis();
clap++;
}
}
if (millis()-detection_range_start >= 400)
{
if (clap == 2)
{
if (!lamps)
{
digitalWrite(relay1,HIGH);
lamps += 1;
}
else if (lamps > 0 && lamps < 2)
{
digitalWrite(relay2,HIGH);
digitalWrite(relay1,LOW);
  lamps += 1;
}else if (lamps > 0 && lamps < 3)
{
digitalWrite(relay1,HIGH);
digitalWrite(relay2,HIGH);

  lamps += 1;
}else if (lamps == 3){
  digitalWrite(relay1,LOW);
 digitalWrite(relay2,LOW);
 lamps = 0;
 }
}
clap = 0;
}
}

void temp() {
  float t = dht.readTemperature();
  if (t > 25) {
    digitalWrite(fan, HIGH);
  } else {
    digitalWrite(fan, LOW);
  }
}



void key_pad() {
  char key = keypad.getKey();
  if (key) {
    tone(buzzer,350);
    delay(500);
    noTone(buzzer);
    if (key == '*') {
      input_password = "";
    } else if (key == '#') {
      if (password == input_password) {
        input_password = "";

  for (pos = 0; pos <= 130; pos += 1) {
            myservo.write(pos);
           delay(15);
        }      
delay(3000);
for (pos = 130; pos >= 0; pos -= 1) {
            myservo.write(pos);
            delay(15);
          }

        
//        if (on == false) {
//          on = true;
//          
//          for (pos = 0; pos <= 130; pos += 1) {
//            myservo.write(pos);
//            delay(15);
//          }
//        }
//        else if (on == true) {
//          on = false;
//                    
//
//          for (pos = 130; pos >= 0; pos -= 1) {
//            myservo.write(pos);
//            delay(15);
//          }
//
//        }

      } else {
        tone(buzzer, 350);
        delay(2000);
        noTone(buzzer);
      }

      input_password = "";
    } else {
      input_password += key;
    }
  }
}
