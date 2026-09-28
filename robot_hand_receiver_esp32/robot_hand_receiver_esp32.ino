#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

const int servoPorts[5] = {0, 3, 4, 7, 8}; // index 0=Thumb,1=Index,2=Middle,3=Ring,4=Pinky
#define SERVOMIN 150
#define SERVOMAX 500
// Per-finger mid (state 1): Thumb, Index, Middle, Ring, Pinky
const int servoMid[5] = {400, 450, 450, 400, 250};

typedef struct {
  uint8_t states[5]; // glove order: Pinky, Ring, Middle, Index, Thumb
} FingerData;

FingerData data;
volatile bool newData = false;

void onReceive(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));
  newData = true;
}

void setFinger(int portIndex, int state) {
  int pulse;
  if (portIndex == 0) { // Thumb - 2 states
    pulse = (state == 2) ? SERVOMAX : SERVOMIN;
  } else {
    if (state == 0) pulse = SERVOMIN;
    else if (state == 1) pulse = servoMid[portIndex];
    else pulse = SERVOMAX;
  }
  pwm.setPWM(servoPorts[portIndex], 0, pulse);
}

void setup() {
  Serial.begin(115200);

  Wire.begin(21, 22);
  pwm.begin();
  pwm.setPWMFreq(60);
  delay(10);

  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onReceive);
  Serial.println("Hand ready, waiting for data...");
}

void loop() {
  if (newData) {
    newData = false;

    // glove: states[0]=Pinky,1=Ring,2=Middle,3=Index,4=Thumb
    // ports:  index 0=Thumb,1=Index,2=Middle,3=Ring,4=Pinky
    setFinger(0, data.states[4]); // Thumb
    setFinger(1, data.states[3]); // Index
    setFinger(2, data.states[2]); // Middle
    setFinger(3, data.states[1]); // Ring
    setFinger(4, data.states[0]); // Pinky

    Serial.print("Pinky:");  Serial.print(data.states[0]);
    Serial.print(" Ring:");  Serial.print(data.states[1]);
    Serial.print(" Middle:"); Serial.print(data.states[2]);
    Serial.print(" Index:"); Serial.print(data.states[3]);
    Serial.print(" Thumb:"); Serial.println(data.states[4]);
  }
}