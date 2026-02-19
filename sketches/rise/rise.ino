#include <ESP32Servo.h>
/*stable platform config. jaottelu & reverse*/
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

constexpr int SERVO1_PIN = 19;
constexpr int SERVO2_PIN = 22;
constexpr int SERVO3_PIN = 23;
constexpr int SERVO4_PIN = 18;

unsigned long lastStepTime = 0;

int  targetPos1;
int  targetPos2;
int  targetPos3;
int  targetPos4;

int dir;

int  initPos1;
int  initPos2;
int  initPos3;
int  initPos4;

enum Mode {
  IDLE,
  RISE,
  LOWER
};

Mode mode = IDLE;

char cmdBuf[32];
uint8_t cmdIdx = 0;

void setAll(int a, int b, int c, int d){
  servo1.write(a);
  servo2.write(b);
  servo3.write(c);
  servo4.write(d);
  delay(2000);
}
void initRise(){

  targetPos1 = 180;
  targetPos2 = 70;
  targetPos3 = 130;
  targetPos4 = 85;

  dir = 1;

  initPos1 = 0;
  initPos2 = 0;
  initPos3 = 0;
  initPos4 = 20;

  setAll(initPos1, initPos2, initPos3, initPos4);
}

void initLower(){

  targetPos1 = 0;
  targetPos2 = 0;
  targetPos3 = -1;
  targetPos4 = 20;

  dir = -1;

  initPos1 = 0;
  initPos2 = 70;
  initPos3 = 135;
  initPos4 = 85;

  setAll(initPos1, initPos2, initPos3, initPos4);
}

void rise(){

  if (millis() - lastStepTime < 2) return;
  lastStepTime = millis();

  //servo1.write(initPos1);
  servo2.write(initPos2);
  servo3.write(initPos3);
  servo4.write(initPos4);

  /*if(abs(initPos1 - targetPos1) > 1){
    initPos1 += dir;
  }*/
  if(abs(initPos2 - targetPos2) > 1){
    initPos2 += dir;
  }
  if(abs(initPos3 - targetPos3) > 1){
    initPos3 += 2*dir;
  }
  if(abs(initPos4 - targetPos4) > 1){
    initPos4 += dir;
  }
  if(abs(initPos3 - targetPos3) <= 1){
    Serial.println("manouver done");
    mode = IDLE;
  }
}

void handleCommand(const char *cmd) {

  Serial.print("Command received: ");
  Serial.println(cmd);

  if (!strcmp(cmd, "rise")) {
    mode = RISE;
    initRise();
    delay(200);
  }  
  if (!strcmp(cmd, "low")) {
    mode = LOWER;
    initLower();
    delay(200);
  }
  if (!strcmp(cmd, "mode")) {
    Serial.println(mode);
  }

  Serial.println("enter command:");
}

void setup() {
  Serial.begin(115200);
  delay(200);
  while (Serial.available()) Serial.read();



  Serial.setRxBufferSize(256);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  /*  */
  servo1.setPeriodHertz(50);
  servo2.setPeriodHertz(50);
  servo3.setPeriodHertz(50);
  servo4.setPeriodHertz(50);

  servo1.attach(SERVO1_PIN, 500, 2500);
  servo2.attach(SERVO2_PIN, 500, 2500);
  servo3.attach(SERVO3_PIN, 500, 2500);
  servo4.attach(SERVO4_PIN, 500, 2500);

  Serial.println("enter command:");
}

void loop() {

  while (Serial.available()) {
    char c = Serial.read();

    if (c >= 'A' && c <= 'Z') c += 32;

    if (c == '\n' || c == '\r') {
      if (cmdIdx > 0) {
        cmdBuf[cmdIdx] = '\0';
        handleCommand(cmdBuf);
        cmdIdx = 0;
      }
    } else if (cmdIdx < sizeof(cmdBuf) - 1) {
      cmdBuf[cmdIdx++] = c;
    }
  }

  switch(mode){
    case RISE:
      rise();
      break;
    case LOWER:
      rise();
      break;
    case IDLE:
      default:
      break;
  }
}

