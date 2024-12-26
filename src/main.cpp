#include <Arduino.h>

#include <ESP32Servo.h>
#include "common.h"
#include "controllers/Mp3Controller.h"

#define MAIN_TAG "Main"
#define DEBUG

void runMs(uint32_t delayUs = 800);

void adjustPosition();

void scene1();

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

  ESP_LOGI(MAIN_TAG, "Setup Beam");
  ledcSetup(CH_BEAM, 1000, 8);
  ledcAttachPin(PIN_BEAM, CH_BEAM);
  BEAM_TURN_OFF();

  ESP_LOGI(MAIN_TAG, "Setup Gundam Gatling");
  ledcSetup(CH_GUNDAM_GATLING, 1000, 8);
  ledcAttachPin(PIN_GUNDAM_GATLING, CH_GUNDAM_GATLING);
  ledcWrite(CH_GUNDAM_GATLING, 0);

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

  ESP_LOGI(MAIN_TAG, "Find starting position...");
  adjustPosition();

  ESP_LOGD(MAIN_TAG, "Wait 3 seconds...");
  delay(1000 * 3);

  {
    GUNDAM_EYE_TURN_ON();

    // Rotate the body
    START_MOTOR();
    for (int i = 0; i < STEPS_PER_REV; i++) {
      runMs();
    }
    STOP_MOTOR();
  }
}

int sceneNo = 0;

void loop() {
  if (sceneNo == 0) {
    scene1();
  } else if (sceneNo == 1) {
    ESP_LOGI(MAIN_TAG, "Find starting position...");
    adjustPosition();
  } else {
    START_MOTOR();
    for (int i = 0; i < STEPS_PER_REV; i++) {
      runMs();
    }
    STOP_MOTOR();
  }

  sceneNo = (++sceneNo) % 4;
}

void adjustPosition() {
  initialized = false;
  START_MOTOR();
  while (!initialized) {
    runMs(1000);
  }
  // More 15 degree
  for (int i = 0; i < STEPS_PER_REV * 15 / 360; i++) {
    runMs();
  }
  STOP_MOTOR();
}

void scene1() {
  delay(1000);

  // Bean On
  BEAM_TURN_ON();
  delay(1000);

  // Face Left
  for (auto i = NECK_ANGLE_START; i <= NECK_ANGLE_LEFT; i += 5) {
    neckServo.write(i);
    delay(50);
  }
  neckServo.write(NECK_ANGLE_LEFT);

  // Wait...
  delay(1000);

  // Gatling
  GUNDAM_EYE_EMPHASIZE();
  delay(800);
  playGatling();
  delay(500);
  for (auto i = 0; i < 5; i++) {
    GUNDAM_GATLING_TURN_ON();
    delay(100);
    GUNDAM_GATLING_TURN_OFF();
    delay(100);
  }
  delay(300);
  stopGatling();
  GUNDAM_EYE_TURN_ON();

  //  Wait...
  delay(1000);

  // Face Front
  for (auto i = NECK_ANGLE_LEFT; i >= NECK_ANGLE_START; i -= 5) {
    neckServo.write(i);
    delay(50);
  }
  neckServo.write(NECK_ANGLE_START);

  //  Wait...
  delay(1000);

  // Bean Off
  BEAM_TURN_OFF();

  //  Wait...
  delay(1000);
}

void runMs(uint32_t delayUs) {
  digitalWrite(PIN_STEP_MOTOR_STEP, HIGH);
  delayMicroseconds(delayUs);
  digitalWrite(PIN_STEP_MOTOR_STEP, LOW);
  delayMicroseconds(delayUs);
}