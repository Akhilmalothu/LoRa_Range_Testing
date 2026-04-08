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
## Pinout Tables
**Sender node 1**

| ESP32 Pin | Component Pin | Component | Function |
| :--- | :--- | :--- | :--- |
| **3V3** | VCC | SX1278 & DHT11 | Power |
| **GND** | GND | SX1278 & DHT11 | Ground |
| **GPIO 18** | SCK | SX1278 | SPI Clock |
| **GPIO 19** | MISO | SX1278 | SPI Master In Slave Out |
| **GPIO 23** | MOSI | SX1278 | SPI Master Out Slave In |
| **GPIO 5** | NSS / CS | SX1278 | SPI Chip Select |
| **GPIO 14** | RST | SX1278 | Reset |
| **GPIO 26** | DIO0 | SX1278 | Digital I/O (Interrupt) |
| **GPIO 4** | DATA | DHT11 | Sensor Data Line |

**Recever node 2**
|ESP32 Pin|Component Pin|Component|Function|
| :--- | :--- | :--- | :--- |
|**3V3**|VCC|SX1278 & OLED|Power|
|**GND**|GND|SX1278 & OLED|Ground|
|**GPIO 5**|SCK|SX1278|SPI Clock|
|**GPIO 19**|MISO|SX1278|SPI Master In Slave Out|
|**GPIO 27**|MOSI|SX1278|SPI Master Out Slave In|
|**GPIO 18**|NSS / CS|SX1278|SPI Chip Select|
|**GPIO 14**|RST|SX1278|Reset|
|**GPIO 26**|DIO0|SX1278|Digital I/O (Interrupt)|
|**GPIO 21**|SDA|SH1106G OLED|I2C Data|
|**GPIO 22**|SCL|SH1106G OLED|I2C Clock|

## Circuit & Wiring Guidelines
**Breadboard Assembly Instructions**

Transmitter Node Setup:

Place the ESP32 securely across the center trench of the breadboard.

Connect the ESP32's 3V3 pin to the red power rail and GND to the blue ground rail.

Mount the SX1278 module and connect its VCC and GND to the respective power rails.

Route jumper wires for the SPI interface: SCK to Pin 18, MISO to Pin 19, MOSI to Pin 23, and NSS to Pin 5.

Connect the LoRa module's RST to Pin 14 and DIO0 to Pin 26.

Mount the DHT11 sensor. Connect its power and ground to the rails.

Connect the DHT11 data pin to ESP32 Pin 4.

Crucial: Place a 10kΩ pull-up resistor between the DHT11 data pin and the 3V3 power rail to ensure stable sensor readings.

**Receiver Node Setup:**

Place the second ESP32 across the center trench of a new breadboard.

Establish the 3V3 and GND rails from the ESP32.

Mount the receiver's SX1278 module, connecting power and ground.

Route the alternate SPI wiring: SCK to Pin 5, MISO to Pin 19, MOSI to Pin 27, and NSS to Pin 18.

Connect the LoRa module's RST to Pin 14 and DIO0 to Pin 26.

Mount the SH1106G OLED display and supply it with 3.3V and GND from the rails.

Connect the OLED's SDA pin to ESP32 Pin 21 and the SCL pin to ESP32 Pin 22.

## Bill of Materials (BOM)

|**Item**|**Qty**|**Description**|**Notes**|
| :--- | :--- | :--- | :--- |
|SX1278 LoRa Module|2|433MHz Transceiver|Includes spring antennas; upgrade to SMA antennas for better range testing.|
|DHT11 Sensor|1|Temperature & Humidity Sensor|Can be swapped for DHT22 for higher precision.|
|SH1106G OLED Display|1|"1.3"" I2C Monochrome Display"|Address usually defaults to 0x3C.|
|10kΩ Resistor|1|Pull-up resistor|Required for the DHT11 data line.|
|Breadboards|2|400 or 830 tie-point|One for each node.|
|Jumper Wires|~30|Male-to-Male and Male-to-Female|Assorted lengths for clean routing.|
