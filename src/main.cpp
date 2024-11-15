#include <Arduino.h>

const int STEP = 3;
const int ENABLE = 4;
const int HALL_SENSOR = 5;

const int steps_per_rev = 2048 /* 200 */;

volatile bool initialized = false;

void IRAM_ATTR InitPos() {
  Serial.println("Catch!");
  initialized = true;
}

void setup() {
  Serial.begin(115200);
  pinMode(STEP, OUTPUT);
  pinMode(ENABLE, OUTPUT);

  gpio_set_direction((gpio_num_t) HALL_SENSOR, GPIO_MODE_INPUT);
  attachInterrupt(HALL_SENSOR, InitPos, RISING);

  digitalWrite(ENABLE, LOW);
  while (!initialized) {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(1000);
    digitalWrite(STEP, LOW);
    delayMicroseconds(1000);
  }
  digitalWrite(ENABLE, HIGH);
  Serial.println("Initialized");

  delay(1000 * 3);
}

void loop() {
  Serial.println("Spinning Clockwise...");

  digitalWrite(ENABLE, LOW);
  for (int i = 0; i < steps_per_rev; i++) {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(800);
    digitalWrite(STEP, LOW);
    delayMicroseconds(800);
  }
  digitalWrite(ENABLE, HIGH);

  delay(1000 * 3);
}