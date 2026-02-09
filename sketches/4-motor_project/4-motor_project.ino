#include <ESP32Servo.h>
/**/
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

constexpr int SERVO1_PIN = 19;
constexpr int SERVO2_PIN = 18;
constexpr int SERVO3_PIN = 23;
constexpr int SERVO4_PIN = 22;

char cmdBuf[32];
uint8_t cmdIdx = 0;


enum Mode {
  IDLE,
  ANGLE
};

Mode mode = IDLE;


// timing
unsigned long lastStepTime = 0;

// routine state
uint8_t routineStep = 0;

// sweep state
int sweepPos = 0;
int sweepDir = 1;

void setAll(int a, int b, int c, int d) {
  servo1.write(a);
  servo2.write(b);
  servo3.write(c);
  servo4.write(d);
}

void setOne(char M, int ang) {
    if(M == 'a'){
        servo1.write(ang);
    }
    if(M == 'b'){
        servo2.write(ang);
    }
    if(M == 'c'){
        servo3.write(ang);
    }
    if(M == 'd'){
        servo4.write(ang);
    }
}


void handleCommand(const char *cmd) {

  Serial.print("Command received: ");
  Serial.println(cmd);

  if(mode == ANGLE){
    if (!strcmp(cmd, "q")) {
      mode = IDLE;
      return;
    }
    int angle;
    char motor;
    
    if (sscanf(cmd, " %c %d", &motor, &angle) == 2) {

      if (motor >= '1' && motor <= '4') {
        motor = 'a' + (motor - '1');
      }

      if (motor < 'a' || motor > 'd' || angle < 0 || angle > 180) {
        Serial.println("invalid motor or angle");
        Serial.println("format: a 90");
        return;
      }
    }

    setOne(motor, angle);
    Serial.print("motor ");
    Serial.print(motor);
    Serial.print(" -> ");
    Serial.println(angle);
    return;

  }
  
  else{
    if (!strcmp(cmd, "180")) {
      setAll(180, 180, 180, 180);
    }
    else if (!strcmp(cmd, "0")) {
      setAll(0, 0, 0, 0);
    }
    else if (!strcmp(cmd, "90")) {
      setAll(90, 90, 90, 90);
    }
    else if (!strcmp(cmd, "up")) {
      setAll(180, 77, 130, 70);
    }
    else if (!strcmp(cmd, "angle")) {
      mode = ANGLE;
    }
    else {
      Serial.println("unknown command");
    }
  }
  if(mode == ANGLE){
    Serial.println("enter motor [1,4]");
  }
  else{
    Serial.println("enter command:");
  }
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
  }
  else if (cmdIdx < sizeof(cmdBuf) - 1) {
    cmdBuf[cmdIdx++] = c;
  }
}


  switch (mode) {
    case ANGLE:
      break;
    case IDLE:
    default:
      break;
  }
}
