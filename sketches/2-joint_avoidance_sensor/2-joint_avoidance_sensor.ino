#include <ESP32Servo.h>
#include <Wire.h>
#include <cstdlib>
#include <ctime>
/* Simple tester code for an IR avoidance sensor connected to a 2-joint arm.*/
Servo myservo1;
Servo myservo2;

int servo1Pin = 19;
int servo2Pin = 18;
int IRPin = 17;

int preAngle1 = 999; //initial value set high to pass the first while-loop.
int preAngle2 = 999;

int angle1;
int angle2;

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

void move(){

  srand(time(0));
  while(!(abs(angle1-preAngle1) >= 20) && preAngle1 < 999){ // Buffer value of 20 degs to ensure that the arm
    angle1 = rand() % 181;                                  // moves a meaninguf amount.
  }

  srand(time(0));
  while(!(abs(angle2-preAngle2) >= 20) && preAngle2 < 999){
    angle2 = rand() % 181;
  }
  
  preAngle1 = angle1;
  myservo1.write(angle1);
  
  preAngle2 = angle2;
  myservo2.write(angle2);

  Serial.println(angle1);
  Serial.println(angle2);
  delay(100);
}

void loop() {
  bool status = digitalRead(IRPin);

  while(!status){
    move();
    status = true;
  }
}
