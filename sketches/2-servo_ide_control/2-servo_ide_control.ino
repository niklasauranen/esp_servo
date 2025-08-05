#include <ESP32Servo.h>
#include <Wire.h>
/*simple interface for testing servo motor control on esp32. Currently 
implemented for 2 180-deg 500-2500us 50hz servos*/

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
  myservo1.write(180);
  myservo2.write(180);
  delay(2000);
  myservo1.write(0);
  myservo2.write(0);
  delay(2000);

  Serial.println("routine done");
}

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

void angle(){
  int number;
  bool setangle = true;
  while(setangle){
    bool numerical = false;
    Serial.println("enter angle (q to quit)");
    while (Serial.available()==0){}
    String input = Serial.readStringUntil('\n');
    input.trim();
    Serial.println(input);

    if(input == "q" || input == "Q"){
      setangle = false;
    }
    else{
      number = input.toInt();
      numerical = true;
    }
    if(number >= 0 && number <= 180){
      myservo1.write(number);
      myservo2.write(number);
    }
    else if(numerical){
      Serial.println("Unknown angle, enter a value between 0-180 deg.");
    }
  }
}

void loop() {
  Serial.println("enter command: ");
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
    Serial.println("unknown command");
  }
  delay(150);
}
