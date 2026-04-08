# ESP32 LoRa Range Testing & Power Save

This repository contains the Arduino sketches used for testing the range and power consumption of ESP32-based LoRa modules. The project was built to test point-to-point communication reliability, data throughput, and power-saving strategies over a 500-meter line-of-sight distance.

## Project Structure

The code is divided into four main folders covering the two primary stages of the project: Range Testing and Power Optimization.

### 1. Range Testing (`P1_RangeTest` & `P2_RangeTest`)
Used to establish baseline communication range, Signal-to-Noise Ratio (SNR), and Received Signal Strength Indicator (RSSI) over distance.
* **`P1_RangeTest` (Transmitter):** Sends a ping packet containing DHT11 sensor data (Temperature & Humidity) every 2 seconds. Evaluates if an ACK response is successfully received back from the receiver.
* **`P2_RangeTest` (Receiver):** Constantly listens for incoming packets. When a ping is received, it displays the sensor data, RSSI, and SNR on an OLED screen, and immediately sends an ACK back to the transmitter.

### 2. Power Optimization (`P1_Sender_PowerSave` & `P2_Receiver_PowerSave`)
*These sketches build upon the range testing code but implement power-saving features to maximize battery life for remote deployments.*
* Implements deep sleep modes.
* Turns off the radio when not actively transmitting or receiving.
* Optimizes the transmission window.

## LoRa Configuration Highlights
* **Frequency:** 433 MHz
* **TX Power:** 20 dBm (Max power for range boosting)
* **Spreading Factor:** SF12 (Maximized for range at the cost of data rate)
* **Signal Bandwidth:** 62.5 kHz

## Hardware Used
* 2x ESP32 LoRa Boards (with integrated SX1278 chips)
* 1x DHT11 Temperature & Humidity Sensor
* 1x SH1106G OLED Display
