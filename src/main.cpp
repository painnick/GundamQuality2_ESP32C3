#include <Arduino.h>

#include "common.h"

#define MAIN_TAG "Main"

const int steps_per_rev = 2048 /* 200 */;

volatile bool initialized = false;

void IRAM_ATTR InitPos() {
  Serial.println("Catch!");
  initialized = true;
}

void setup() {
  ESP_LOGI(MAIN_TAG, "Start!");
  pinMode(PIN_STEP_MOTOR_STEP, OUTPUT);
  pinMode(PIN_STEP_MOTOR_ENABLE, OUTPUT);

  gpio_set_direction((gpio_num_t) PIN_HALL_SENSOR, GPIO_MODE_INPUT);
  attachInterrupt(PIN_HALL_SENSOR, InitPos, RISING);

  digitalWrite(PIN_STEP_MOTOR_ENABLE, LOW);
  while (!initialized) {
    digitalWrite(PIN_STEP_MOTOR_STEP, HIGH);
    delayMicroseconds(1000);
    digitalWrite(PIN_STEP_MOTOR_STEP, LOW);
    delayMicroseconds(1000);
  }
  digitalWrite(PIN_STEP_MOTOR_ENABLE, HIGH);
  ESP_LOGI(MAIN_TAG, "Initialize the step motor.");

  ESP_LOGD(MAIN_TAG, "Wait 3 seconds...");
  delay(1000 * 3);
}

void loop() {
  digitalWrite(PIN_STEP_MOTOR_ENABLE, LOW);
  for (int i = 0; i < steps_per_rev; i++) {
    digitalWrite(PIN_STEP_MOTOR_STEP, HIGH);
    delayMicroseconds(800);
    digitalWrite(PIN_STEP_MOTOR_STEP, LOW);
    delayMicroseconds(800);
  }
  digitalWrite(PIN_STEP_MOTOR_ENABLE, HIGH);

  ESP_LOGD(MAIN_TAG, "Wait 3 seconds...");
  delay(1000 * 3);
}