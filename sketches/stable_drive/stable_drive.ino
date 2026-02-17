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

char cmdBuf[32];
uint8_t cmdIdx = 0;


enum Mode {
  IDLE,
  ANGLE,
  SWEEP,
  RSWEEP
};

Mode mode = IDLE;


// timing
unsigned long lastStepTime = 0;

// routine state
uint8_t routineStep = 0;

// sweep state
int sweepPos1 = 0;
int sweepPos2 = 0;
int sweepPos3 = 0;
int sweepPos4 = 0;

int phase1targetPos1;
int phase1targetPos4;

int phase2targetPos2;
int phase2targetPos4;

int phase3targetPos3;
int phase3targetPos4;

bool phase_1 = false;
bool phase_2 = false;
bool phase_3 = false;

int sweepDir;
int step = 0;
int prestep=0;

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

void startSweep() {
  mode = SWEEP;
  sweepPos1 = 180;
  sweepPos2 = 70;
  sweepPos3 = 130;
  sweepPos4 = 85;


  phase1targetPos1 = 0;
  phase1targetPos4 = 76;

  phase2targetPos2 = 0;
  phase2targetPos4 = 158;

  phase3targetPos3 = 0;
  phase3targetPos4 = 20;
  
  phase_1 = true;
  phase_2 = false;
  phase_3 = false;

  Serial.println(phase_1);
  Serial.println(phase_2);
  Serial.println(phase_3);

  sweepDir = 1;
  

  setAll(sweepPos1,sweepPos2,sweepPos3,sweepPos4);

  lastStepTime = millis();
}

void startRSweep() {
  mode = RSWEEP;
  sweepPos1 = 0;
  sweepPos2 = 0;
  sweepPos3 = 0;
  sweepPos4 = 20;


  phase3targetPos3 = 130;
  phase3targetPos4 = 158;

  phase2targetPos2 = 70;
  phase2targetPos4 = 76;

  phase1targetPos1 = 180;
  phase1targetPos4 = 85;

  Serial.println(phase_1);
  Serial.println(phase_2);
  Serial.println(phase_3);
  
  phase_1 = false;
  phase_2 = false;
  phase_3 = true;

  sweepDir = -1;
  

  setAll(sweepPos1,sweepPos2,sweepPos3,sweepPos4);

  lastStepTime = millis();
}

void Phase1() {
  if(phase_1 && !phase_2 && !phase_3){
    Serial.println("on 1");
    if (millis() - lastStepTime < 2) return;
    lastStepTime = millis();
    
      servo1.write(sweepPos1);
      servo4.write(sweepPos4);

      if(abs(sweepPos1 - phase1targetPos1) > 1){
        sweepPos1 -= sweepDir;
        step +=1;

      }
      if(abs(sweepPos4 - phase1targetPos4) > 1 && abs(step-prestep) >= 20){
        sweepPos4 -= sweepDir;
        prestep = step;
      }

      if (abs(sweepPos1 - phase1targetPos1) <= 2 && abs(sweepPos4 - phase1targetPos4) <= 2) {
        Serial.println("Phase 1 done");
        step=0;
        prestep=0;
        if(mode == RSWEEP){
          mode = IDLE;
        }

        else{
          phase_1 = false;
          phase_2 = true;
        }
      }
    }
    
    else{
      return;
    }
}

void Phase2(){
  if(!phase_1 && phase_2 && !phase_3){
   Serial.println("on 2");
    servo2.write(sweepPos2);
    servo4.write(sweepPos4);
    
    if(abs(sweepPos4 - phase2targetPos4) > 2){
      sweepPos4 += sweepDir;
    }

    if(abs(sweepPos2 - phase2targetPos2) > 1 && step%4 != 0){
      sweepPos2 -= sweepDir;
      }

    step +=1;

    if (abs(sweepPos2 - phase2targetPos2) <= 2 && abs(sweepPos4 - phase2targetPos4) <= 2) {
      Serial.println("phase 2 done");
      step = 0;
      prestep = 0;
      phase_2 = false;
      if(mode == RSWEEP){
        phase_1 = true;  
      }
      else{
        phase_3 = true;
      }
    }
  }
  else{
    return;
  }
}

void Phase3(){

  if (millis() - lastStepTime < 2) return;
    lastStepTime = millis();

    if(!phase_1 && !phase_2 && phase_3){
      Serial.println("on 3");
      servo3.write(sweepPos3);
      servo4.write(sweepPos4);
    if(abs(sweepPos3 - phase3targetPos3 ) >=2 && step%40 != 0){

      sweepPos3 -= sweepDir;
    
    }

    if(abs(sweepPos4 - phase3targetPos4) >=2){
      sweepPos4 -= sweepDir;
      step +=1;
    }
    if (abs(sweepPos3 - phase3targetPos3) <= 2 && abs(sweepPos4 - phase3targetPos4) <= 2) {
      Serial.println("phase 3 done");
      if(mode == SWEEP){
        mode = IDLE;
      }

      else{
        phase_3 = false;
        phase_2 = true;
      }
    }
  }
  else{
    return;
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
      setAll(180, 70, 130, 85);
    }
    else if (!strcmp(cmd, "upl")) {
      setAll(0, 70, 130, 76);
    }
    else if (!strcmp(cmd, "down")) {
      setAll(0, 0, 0, 20);
    }
    else if (!strcmp(cmd, "p1")) {
      setAll(0, 70, 130, 76);
    }
    else if (!strcmp(cmd, "p2")) {
      setAll(0, 0, 130, 158);
    }
    else if (!strcmp(cmd, "p3")) {
      setAll(0, 0, 0, 25);
    }
    else if (!strcmp(cmd, "angle")) {
      mode = ANGLE;
    }  
    else if (!strcmp(cmd, "sweep")) {
      startSweep();
    }
    else if (!strcmp(cmd, "rsweep")) {
      startRSweep();
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
    case SWEEP:
      Phase1();
      Phase2();
      Phase3();
      break;
    case RSWEEP:
      Phase3();
      Phase2();
      Phase1();
      break;
    case IDLE:
    default:
      break;
  }
}
	