
#include <ESP32Servo.h> 
#include <cmath>
#include <Wire.h>
Servo myservo1;
Servo myservo2;
int servo1Pin = 18;
int servo2Pin = 19;

bool useServo1 = true;
bool lastButtonState = HIGH;

int buttonPin = 23;
int potPin = 2;      
float ADC_Max = 4095.0;
  
float val;
int ScaledVal;
int PreVal1 = 0;
int PreVal2 = 0;
int buffer = 10;

unsigned long debounceDelay = 500;

void setup()
{
  Serial.begin(115200);
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);

  myservo1.setPeriodHertz(50);
  myservo2.setPeriodHertz(50);
  myservo1.attach(servo1Pin, 500, 2500);
  myservo2.attach(servo2Pin, 500, 2500);

  pinMode(buttonPin, INPUT_PULLUP);
}

boolean debounceButton(boolean state){

  boolean stateNow = digitalRead(buttonPin);
  if(state!=stateNow)
  {
    delay(debounceDelay);
    stateNow = digitalRead(buttonPin);
  }
  return stateNow;
}


void loop() {

  bool reading = digitalRead(buttonPin);
  if(debounceButton(reading) == LOW && lastButtonState == HIGH){
      
    useServo1 = !useServo1;
    Serial.println("jiasdjasdasd");
  }
  
  
  int val = analogRead(potPin);

  ScaledVal = map(val, 0, ADC_Max, 0, 180);
  ScaledVal = (ScaledVal + 2)/7*7;
  
  if (useServo1) {
    myservo2.detach();
    //if(PreVal1-20 <= ScaledVal && ScaledVal <= PreVal1 + 20){
      if(abs(ScaledVal - PreVal1) >= buffer){
          myservo1.attach(servo1Pin, 500, 2500);
          Serial.println("Servo 1    ");
          Serial.print(ScaledVal);
          Serial.print(" ");
          Serial.print(PreVal1);
          Serial.println("");
          myservo1.write(ScaledVal);
          PreVal1 = ScaledVal;
      }
    //}
  } 
  else {
    myservo1.detach();
    //if((PreVal2 - 25) <= ScaledVal  && ScaledVal <= (PreVal2 + 25)){
        if(abs(ScaledVal - PreVal2) >= buffer){
            myservo2.attach(servo2Pin, 500, 2500);
            Serial.println("Servo 2    "); 
            Serial.print(ScaledVal);
            Serial.print(" ");
            Serial.print(PreVal2);
            Serial.println("");
            myservo2.write(ScaledVal);
            PreVal2 = ScaledVal;
        }
      
    //}
  }

  delay(150); 
}


