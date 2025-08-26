#include <ESP32Servo.h>
#include <Wire.h>
#include <cmath>
#include <stdio.h>
/*simple interface for testing servo motor control on esp32. Currently 
implemented for 2 180-deg 500-2500us 50hz servos*/

Servo myservo1;
Servo myservo2;

int servo1Pin = 19;
int servo2Pin = 18;
int IRPin = 17;
int number1;
int number2;
int preangle1;
int preangle2;
float degfactor = 6.6667;
String com ="";


void setup() {

  Serial.begin(115200);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  myservo1.setPeriodHertz(50);  
  myservo1.attach(servo1Pin,500,2500);  

  myservo2.setPeriodHertz(50);
  myservo2.attach(servo2Pin,500,2500);  
  
  pinMode(IRPin, INPUT);
}

void routine(){
  
  myservo1.write(0);
  myservo2.write(0);
  if(!digitalRead(IRPin)){
    Serial.println("Seen something at 0, 0");
  }
  delay(2000);
  myservo1.write(90);
  myservo2.write(90);
  if(!digitalRead(IRPin)){
    Serial.println("Seen something at 90, 90");
  }
  delay(2000);
  myservo1.write(180);
  myservo2.write(180);
  if(!digitalRead(IRPin)){
    Serial.println("Seen something at 180, 180");
  }
  delay(2000);
  myservo1.write(0);
  myservo2.write(0);
  if(!digitalRead(IRPin)){
    Serial.println("Seen something at 0, 0");
  }
  delay(2000); 
  myservo1.write(90);
  myservo2.write(90);
  if(!digitalRead(IRPin)){
    Serial.println("Seen something at 90, 90");
  }
  delay(2000);
  myservo1.write(180);
  myservo2.write(180);
  if(!digitalRead(IRPin)){
    Serial.println("Seen something at 180, 180");
  }
  delay(2000);
  myservo1.write(0);
  myservo2.write(0);
  if(!digitalRead(IRPin)){
    Serial.println("Seen something at 0, 0");
  }
  delay(2000);

  Serial.println("routine done");
}

void loop() {
  Serial.println("enter command: ");
  while (Serial.available()==0){}
  String com = Serial.readString();
  com.trim();
  if(com=="routine"){
    Serial.println(com);
    routine();
  }
  else if(com=="angle"){
    Serial.println(com);
    angle();
  }
  else{
    Serial.println("unknown command");
  }
  delay(150);
}

void delayscheduling(int angle1, int preangle1, int angle2, int preangle2){

  int cpos = sqrt(angle1*angle1 + angle2*angle2);
  int prepos = sqrt(preangle1*preangle1 + preangle2*preangle2);
  int diff = abs(cpos-prepos);
  float del = diff*degfactor;
  Serial.printf("Angle1 preangle1: %d %d\n", angle1, preangle1);
  Serial.printf("Angle2 preangle2: %d %d\n", angle2, preangle2);
  Serial.printf("current pos %d\n", cpos);
  Serial.printf("prev. pos %d\n", prepos);
  Serial.printf("difference: %d\n", diff);
  Serial.printf("delay (ms), %f\n", del); // simplify to use only on one motor
  delay(del);

}

void engage(int ang1, int ang2){
  Serial.print("engaged at ");
  Serial.print(ang1);
  Serial.println("");
}

void angle(){
  int number;
  bool setangle = true;
  while(setangle){
    Serial.println("enter 1 or 2 for motors 1 and 2 (q to quit)");
    while (Serial.available()==0){}
    String input = Serial.readStringUntil('\n');
    input.trim();
    Serial.println(input);

    if(input == "q" || input == "Q"){
      setangle = false;
      number = 0; // prevents the program from re-entering the motor loops.
    }
    else{
      number = input.toInt();
    }
    if(number == 1){
      bool motor1 = true;
      Serial.println("enter angle for 1 (q to quit)");
      while(motor1){
        while (Serial.available()==0){}
        String input1 = Serial.readStringUntil('\n');
        input1.trim();
        Serial.println(input1);
        if(input1 == "q" || input1 == "Q"){
            motor1 = false;
        }
        else{
          number1 = input1.toInt();
          if(number1 >= 0 && number1 <= 180){
            myservo1.write(number1);
            delayscheduling(number1, preangle1, number2, preangle2);
            preangle1 = number1;
            if(!digitalRead(IRPin)){
              engage(number1, number2);
            }
          }
          else{
            Serial.println("bad number");
          }
        }
      }
    }
  
    else if(number == 2){
      bool motor2 = true;
      Serial.println("enter angle for 2 (q to quit)");
      while(motor2){
        while (Serial.available()==0){}
        String input2 = Serial.readStringUntil('\n');
        input2.trim();
        Serial.println(input2);
        if(input2 == "q" || input2 == "Q"){
          motor2 = false;
        }
        else{
          number2 = input2.toInt();
          if(number2 >= 0 && number2 <= 180){
            myservo2.write(number2);
            delayscheduling(number1, preangle1, number2, preangle2);
            preangle2 = number2;
            if(!digitalRead(IRPin)){
              engage(number1, number2);
            }
          }
          else{
            Serial.println("bad number");
          }
        }
      }
    }
  }
}
/*
void sweep(){
  myservo2.write(0);
  myservo1.write(180);
  delay(1000);
  for(int i =0; i<=180; ++i){
    myservo2.write(i);
    myservo1.write(180-i);
    delay(10);
  }
  for(int i =180; i>=0; --i){
    myservo2.write(i);
    myservo1.write(180-i);
    delay(10);
  }
  Serial.println("sweep done");
}

*/