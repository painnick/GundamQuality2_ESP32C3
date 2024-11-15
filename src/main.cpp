#include <Arduino.h>

#include <ESP32Servo.h>
#include "common.h"

#define MAIN_TAG "Main"

Servo neckServo;

volatile bool initialized = false;

void IRAM_ATTR InitPos() {
  ESP_LOGD(MAIN_TAG, "Catch!");
  initialized = true;
}

void setup() {
  ESP_LOGI(MAIN_TAG, "Start!");

  neckServo.attach(PIN_NECK_SERVO);
  neckServo.write(NECK_ANGLE_START);

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
  for (int i = 0; i < STEPS_PER_REV; i++) {
    digitalWrite(PIN_STEP_MOTOR_STEP, HIGH);
    delayMicroseconds(800);
    digitalWrite(PIN_STEP_MOTOR_STEP, LOW);
    delayMicroseconds(800);
  }
  digitalWrite(PIN_STEP_MOTOR_ENABLE, HIGH);


  ESP_LOGD(MAIN_TAG, "Face Left");
  for (auto i = NECK_ANGLE_START; i <= NECK_ANGLE_LEFT; i += 5) {
    neckServo.write(i);
    delay(50);
  }
  neckServo.write(NECK_ANGLE_LEFT);

  ESP_LOGD(MAIN_TAG, "Wait 2 seconds...");
  delay(2000);

  ESP_LOGD(MAIN_TAG, "Face Front");
  for (auto i = NECK_ANGLE_LEFT; i >= NECK_ANGLE_START; i -= 5) {
    neckServo.write(i);
    delay(50);
  }
  neckServo.write(NECK_ANGLE_START);

  ESP_LOGD(MAIN_TAG, "Wait 3 seconds...");
  delay(1000 * 3);
}