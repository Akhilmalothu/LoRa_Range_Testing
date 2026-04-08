/*
 * === P2 (YOU) SIGNAL TESTER NODE ===
 * Purpose: Always listening. Receives packet, displays Signal Strength & SNR, and sends ACK back to P1.
 * Powersave is OFF for Range Testing.
 */

#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3C // Sometimes 0x3D
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire, OLED_RESET);

// LoRa SPI Pins
#define SCK 5
#define MISO 19
#define MOSI 27
#define ss 18
#define rst 14
#define dio0 26

void setup() {
  Serial.begin(115200);
  while(!Serial);

  Serial.println("\n[P2] Signal Tester Node Starting...");

  delay(250); // Give OLED time to boot up
  if(!display.begin(i2c_Address, true)) {
    Serial.println(F("OLED Init failed"));
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);
  display.println("Signal Tester:");
  display.println("Listening...");
  display.display();

  SPI.begin(SCK, MISO, MOSI, ss);
  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) {
    Serial.println("[P2 ERROR] LoRa failed to start!");
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("LoRa Fail!");
    display.display();
    while (1);
  }
  
  // --- RANGE BOOST SETTINGS (COMMENTED OUT FOR DEBUGGING) ---
  // If your modules were working before, one of these parameters is crashing your specific chip!
  LoRa.setTxPower(20);               
  LoRa.setSpreadingFactor(12);       
  LoRa.setSignalBandwidth(62.5E3);    
  // LoRa.enableCrc();
  
  Serial.println("[P2 STATUS] LoRa Init OK. Constantly listening for P1's pings...");
}

void loop() {
  // Constantly check the airwaves
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String receivedText = "";
    while (LoRa.available()) {
      receivedText += (char)LoRa.read();
    }
    
    // Check if the packet came from P1
    if (receivedText.startsWith("PING")) {
      int firstComma = receivedText.indexOf(',');
      int secondComma = receivedText.indexOf(',', firstComma + 1);
      int thirdComma = receivedText.indexOf(',', secondComma + 1);
      
      if(firstComma > 0 && secondComma > firstComma && thirdComma > secondComma) {
        String pCountStr = receivedText.substring(firstComma + 1, secondComma);
        String tempStr = receivedText.substring(secondComma + 1, thirdComma);
        String humStr = receivedText.substring(thirdComma + 1);
        
        int rssi = LoRa.packetRssi();
        float snr = LoRa.packetSnr(); // Grab the Signal-to-Noise Ratio

        Serial.print("\n[P2 RX] Received Packet #");
        Serial.print(pCountStr);
        Serial.print(" | Temp: ");
        Serial.print(tempStr);
        Serial.print("C | Hum: ");
        Serial.print(humStr);
        Serial.print("% | RSSI: ");
        Serial.print(rssi);
        Serial.print(" | SNR: ");
        Serial.println(snr);

        display.clearDisplay();
        display.setTextSize(2); // Nice big text for the OLED
        display.setCursor(0, 0); 
        display.print("Pkt:"); display.println(pCountStr);
        display.print("R:  "); display.print(rssi); display.println("dBm");
        display.print("SNR:"); display.println(snr);
        display.display();

        // Instantly reply with an ACK back to P1!
        Serial.println("[P2 TX] Sending confirmation ACK back to P1...");
        LoRa.beginPacket();
        LoRa.print("ACK,");
        LoRa.print(pCountStr);
        LoRa.endPacket();
      }
    }
  }
}
