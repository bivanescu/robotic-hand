#include <Wire.h>
#include "MPU6050.h"
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

#define MUX_ADDR 0x70
MPU6050 mpu;

// Hand ESP32 MAC address
uint8_t handAddress[] = {0x70, 0x4B, 0xCA, 0x4D, 0x22, 0xD4};

typedef struct {
  uint8_t states[5]; // Pinky, Ring, Middle, Index, Thumb
} FingerData;

FingerData data;
esp_now_peer_info_t peerInfo;

void selectMuxChannel(uint8_t ch) {
  Wire.beginTransmission(MUX_ADDR);
  Wire.write(1 << ch);
  Wire.endTransmission();
}

int getFingerState(int channel) {
  selectMuxChannel(channel);
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  if (channel == 4) { // Thumb - 2 states
    if (ax > -6200) return 0;
    else return 2;
  }

  int straight, half;
  if (channel == 0) { straight = -8000;  half = -13000; } // Pinky
  if (channel == 1) { straight = -5500;  half = -12000; } // Ring
  if (channel == 2) { straight = -5500;  half = -11500; } // Middle
  if (channel == 3) { straight = -10500; half = -14000; } // Index

  if (ax > straight) return 0;
  else if (ax > half) return 1;
  else return 2;
}

void setup() {
  Wire.begin(22, 21);
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  memcpy(peerInfo.peer_addr, handAddress, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  for (int i = 0; i < 5; i++) {
    selectMuxChannel(i);
    mpu.initialize();
  }
  Serial.println("Glove ready!");
}

void loop() {
  for (int i = 0; i < 5; i++) {
    data.states[i] = getFingerState(i);
  }

  esp_now_send(handAddress, (uint8_t *)&data, sizeof(data));

  Serial.print("Sent: ");
  for (int i = 0; i < 5; i++) { Serial.print(data.states[i]); Serial.print(" "); }
  Serial.println();

  delay(100);
}