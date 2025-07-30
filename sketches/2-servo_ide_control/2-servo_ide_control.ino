#include <ESP32Servo.h>
#include <Wire.h>
Servo myservo1;
Servo myservo2;

int servo1Pin = 18;
int servo2Pin = 19;
int prenumber;
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
}

void routine(){
  
  myservo1.write(0);
  myservo2.write(0);
  delay(2000);
  myservo1.write(90);
  myservo2.write(90);
  delay(2000);
  myservo1.write(180);
  myservo2.write(180);
  delay(2000);
  myservo1.write(0);
  myservo2.write(0);
  delay(2000); 
  myservo1.write(90);
  myservo2.write(90);
  delay(2000);
}

void sweep(){
  myservo2.write(0);
  myservo1.write(180);
  delay(1000);
  for(int i =0; i<=180; ++i){
    myservo2.write(i);
    myservo1.write(180-i);
    delay(50);
  }
  for(int i =180; i>=0; --i){
    myservo2.write(i);
    myservo1.write(180-i);
    delay(50);
  }
}

void angle(){
  int number;
  bool setangle = true;
  while(setangle){
    Serial.println("enter angle");
    while (Serial.available()==0){}
    number = Serial.parseInt();
    Serial.println(number);
    if(number <= 180 && number > 0){
      myservo1.write(number);
      myservo2.write(number);
    }

    else{
      Serial.println("quit angle? (y/n)");
      while (Serial.available()==0){}
      String acom = Serial.readString();
      acom.trim();
      if(acom == "y"){ // fix
        setangle = false;
      }
    }
  }
}

void loop() {
  Serial.println("enter command");
  while (Serial.available()==0){}
  String com = Serial.readString();
  com.trim();

  if(com=="up"){
    Serial.println(com);
    myservo1.write(90);
    myservo2.write(90);
  }
  else if(com=="right"){
    Serial.println(com);
    myservo1.write(0);
    myservo2.write(180);
  }
  else if(com=="left"){
    Serial.println(com);
    myservo1.write(180);
    myservo2.write(0);
  }
  else if(com=="in"){
    Serial.println(com);
    myservo1.write(0);
    myservo2.write(0);
  }
  else if(com=="routine"){
    Serial.println(com);
    routine();
  }
  else if(com=="sweep"){
    Serial.println(com);
    sweep();
  }
  else if(com=="angle"){
    Serial.println(com);
    angle();
  }
  else{
    Serial.println("unknown command brotesserie chicken");
  }
  delay(150);
}
