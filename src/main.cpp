#include <Arduino.h>

#include <ESP32Servo.h>
#include "common.h"
#include "controllers/Mp3Controller.h"

#define MAIN_TAG "Main"
#define DEBUG

Servo neckServo;

volatile bool initialized = false;

void IRAM_ATTR InitPos() {
  ESP_LOGD(MAIN_TAG, "Catch!");
  initialized = true;
}

void setup() {
  ESP_LOGI(MAIN_TAG, "Start!");

  // MP3
  setupSound();
  delay(1000);
  setDefaultVolume();

  playOpening();
  delay(4000);

  // LEDs
  ESP_LOGI(MAIN_TAG, "Setup Gundam Eyes");
  ledcSetup(CH_GUNDAM_EYE, 1000, 8);
  ledcAttachPin(PIN_GUNDAM_EYE, CH_GUNDAM_EYE);
  GUNDAM_EYE_TURN_OFF();

  // Neck
  ESP_LOGI(MAIN_TAG, "Setup Neck Servo motor");
  neckServo.attach(PIN_NECK_SERVO);
  neckServo.write(NECK_ANGLE_START);

  // Rotator
  ESP_LOGI(MAIN_TAG, "Setup Rotator");
  pinMode(PIN_STEP_MOTOR_STEP, OUTPUT);
  pinMode(PIN_STEP_MOTOR_ENABLE, OUTPUT);

  gpio_set_direction((gpio_num_t) PIN_HALL_SENSOR, GPIO_MODE_INPUT);
  attachInterrupt(PIN_HALL_SENSOR, InitPos, RISING);

  ESP_LOGI(MAIN_TAG, "Find starting point...");
  digitalWrite(PIN_STEP_MOTOR_ENABLE, LOW);
  while (!initialized) {
    digitalWrite(PIN_STEP_MOTOR_STEP, HIGH);
    delayMicroseconds(1000);
    digitalWrite(PIN_STEP_MOTOR_STEP, LOW);
    delayMicroseconds(1000);
  }
  digitalWrite(PIN_STEP_MOTOR_ENABLE, HIGH);
  ESP_LOGI(MAIN_TAG, "Found starting point!");

  ESP_LOGD(MAIN_TAG, "Wait 3 seconds...");
  delay(1000 * 3);
}

void loop() {
  GUNDAM_EYE_TURN_ON();

  // Take a turn and get to the starting point.
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

  GUNDAM_EYE_EMPHASIZE();
  playGatling();
  delay(500);
  GUNDAM_EYE_TURN_ON();

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