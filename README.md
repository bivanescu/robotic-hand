# Wireless Robotic Hand

An ESP32-based robotic hand controlled wirelessly by a sensor glove. The glove detects the position of five fingers and sends the detected finger states to a second ESP32 using ESP-NOW. The receiver drives the robotic hand servos through a PCA9685 PWM servo driver.

## How It Works

1. The glove reads finger movement from an MPU6050 accelerometer.
2. An I2C multiplexer selects the sensor channel for each finger.
3. Each finger is converted into a movement state: open, partially closed, or closed.
4. The states are transmitted from the glove ESP32 to the hand ESP32 using ESP-NOW.
5. The hand ESP32 maps the received states to servo positions through the PCA9685 driver.

## Repository Contents

```text
glove_transmitter_esp32/
  glove_transmitter_esp32.ino       # ESP32 transmitter code
robot_hand_receiver_esp32/
  robot_hand_receiver_esp32.ino     # ESP32 receiver and servo control code
RAS-Document.pdf                    # Project documentation
circuit_prototype.jpg               # Circuit prototype
demo.mp4                            # Project demonstration
```

## Hardware

- 2x ESP32 development boards
- MPU6050 accelerometer sensors
- I2C multiplexer
- PCA9685 16-channel PWM servo driver
- 5x servos for the robotic hand
- Sensor glove and connecting wires

## Required Arduino Libraries

Install these libraries through the Arduino IDE Library Manager:

- MPU6050
- Adafruit PWM Servo Driver Library

The ESP-NOW, WiFi, Wire, and ESP WiFi libraries are included with the ESP32 Arduino core.

## Setup

1. Install the ESP32 board package in the Arduino IDE.
2. Install the required libraries listed above.
3. Open `glove_transmitter_esp32/glove_transmitter_esp32.ino` and upload it to the glove ESP32.
4. Open `robot_hand_receiver_esp32/robot_hand_receiver_esp32.ino` and upload it to the hand ESP32.
5. Make sure both ESP32 boards use Wi-Fi channel 1.
6. Check that the receiver MAC address in the transmitter sketch matches the hand ESP32.
7. Power the servos with a suitable external power supply and connect the grounds.

## Notes

- The transmitter and receiver use the same `FingerData` structure and finger order.
- Servo positions and sensor thresholds are calibrated for the prototype hardware and may need adjustment.
- Do not power multiple servos directly from the ESP32 board.
- The project documentation, circuit image, and demonstration video are included in this repository.
