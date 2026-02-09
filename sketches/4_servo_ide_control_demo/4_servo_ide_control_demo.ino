#include <ESP32Servo.h>

// =======================
// Servo setup
// =======================
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

constexpr int SERVO1_PIN = 19;
constexpr int SERVO2_PIN = 18;
constexpr int SERVO3_PIN = 23;
constexpr int SERVO4_PIN = 22;

// =======================
// Serial command buffer
// =======================
char cmdBuf[32];
uint8_t cmdIdx = 0;

// =======================
// Motion state machine
// =======================
enum Mode {
  IDLE,
  ROUTINE,
  SWEEP
};

Mode mode = IDLE;

// timing
unsigned long lastStepTime = 0;

// routine state
uint8_t routineStep = 0;

// sweep state
int sweepPos = 0;
int sweepDir = 1;

// =======================
// Helper functions
// =======================
void setAll(int a, int b, int c, int d) {
  servo1.write(a);
  servo2.write(b);
  servo3.write(c);
  servo4.write(d);
}

void startRoutine() {
  mode = ROUTINE;
  routineStep = 0;
  lastStepTime = millis();
}

void runRoutine() {
  if (millis() - lastStepTime < 2000) return;

  lastStepTime = millis();

  switch (routineStep) {
    case 0: setAll(0, 0, 0, 0); break;
    case 1: setAll(90, 90, 90, 90); break;
    case 2: setAll(180, 180, 180, 180); break;
    case 3: setAll(0, 0, 0, 0); break;
    case 4: setAll(180, 180, 180, 180); break;
    case 5: setAll(90, 90, 90, 90); break;
    case 6: setAll(50, 50, 50, 50); break;
    case 7:
      setAll(0, 0, 0, 0);
      Serial.println("routine done");
      mode = IDLE;
      return;
  }
  routineStep++;
}

void startSweep() {
  mode = SWEEP;
  sweepPos = 0;
  sweepDir = 1;
  lastStepTime = millis();
}

void runSweep() {
  if (millis() - lastStepTime < 10) return;
  lastStepTime = millis();

  servo2.write(sweepPos);
  servo1.write(180 - sweepPos);

  sweepPos += sweepDir;

  if (sweepPos >= 180 || sweepPos <= 0) {
    sweepDir = -sweepDir;
    if (sweepPos <= 0) {
      Serial.println("sweep done");
      mode = IDLE;
    }
  }
}

// =======================
// Command handler
// =======================
void handleCommand(const char *cmd) {
  Serial.print("Command received: ");
  Serial.println(cmd);

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
    setAll(0, 65, 120, 45);
  }
  else if (!strcmp(cmd, "routine")) {
    startRoutine();
  }
  else if (!strcmp(cmd, "sweep")) {
    startSweep();
  }
  else {
    Serial.println("unknown command");
  }

  Serial.println("enter command:");
}

// =======================
// Setup
// =======================
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

// =======================
// Main loop (NON-BLOCKING)
// =======================
void loop() {

  // --- Serial input ---
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


  // --- Motion tasks ---
  switch (mode) {
    case ROUTINE:
      runRoutine();
      break;
    case SWEEP:
      runSweep();
      break;
    case IDLE:
    default:
      break;
  }
}
