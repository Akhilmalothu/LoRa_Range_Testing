/*
 * === P1 (FRiEND) SIGNAL TESTER NODE ===
 * Purpose: Send packet every 2 seconds and wait for an ACK to display Signal Strength and SNR.
 * Powersave is OFF for Range Testing.
 */

#include <SPI.h>
#include <LoRa.h>
#include <DHT.h>

// LoRa SPI Pins
#define SCK 18
#define MISO 19
#define MOSI 23
#define ss 5
#define rst 14
#define dio0 26

// DHT Config
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int packetCount = 0;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  
  Serial.println("\n[P1] Signal Tester Node Starting...");

  dht.begin();
  
  SPI.begin(SCK, MISO, MOSI, ss);
  LoRa.setPins(ss, rst, dio0);
  
  if (!LoRa.begin(433E6)) {
    Serial.println("[P1 ERROR] LoRa failed to start!");
    while(1);
  }
  
  // --- RANGE BOOST SETTINGS (COMMENTED OUT FOR DEBUGGING) ---
  // If your modules were working before, one of these parameters is crashing your specific chip!
  LoRa.setTxPower(20);               
  LoRa.setSpreadingFactor(12);      
  LoRa.setSignalBandwidth(62.5E3);    
  // LoRa.enableCrc();                  
  
  Serial.println("[P1 STATUS] Ready. Transmitting ping every 2 seconds...");
}

void loop() {
  packetCount++;
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  Serial.print("\n[P1 TX] Sending Packet #"); 
  Serial.print(packetCount);
  Serial.print(" | Temp: ");
  Serial.print(t);
  Serial.print("C | Hum: ");
  Serial.print(h);
  Serial.println("% -> Waiting for P2 confirmation (ACK)...");
  
  // Send the Ping Packet
  LoRa.beginPacket();
  LoRa.print("PING,");
  LoRa.print(packetCount);
  LoRa.print(",");
  LoRa.print(t);
  LoRa.print(",");
  LoRa.print(h);
  LoRa.endPacket();

  // Wait for P2 to reply!
  unsigned long waitStart = millis();
  bool ackReceived = false;
  
  // Listen for up to 5 second
  while (millis() - waitStart < 5000) { 
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
      String receivedText = "";
      while (LoRa.available()) {
        receivedText += (char)LoRa.read();
      }
      
      if (receivedText.startsWith("ACK")) {
        int rssi = LoRa.packetRssi();
        float snr = LoRa.packetSnr(); // Grab the Signal-to-Noise Ratio

        Serial.print("[P1 RX] Success! Reached P2 | RSSI Cfg: ");
        Serial.print(rssi);
        Serial.print(" | SNR: ");
        Serial.println(snr);
        ackReceived = true;
      }
      break; 
    }
  }
  
  if (!ackReceived) {
    Serial.println("[P1 ERROR] No reply from P2. Out of range or P2 is off.");
  }

  // Wait another 3 second before doing the next 2-second cycle
  delay(3000); 
}
