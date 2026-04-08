/*
 * === P1 (FRiEND) SENDER NODE ===
 * Components: ESP32 + LoRa SX1278 + DHT Sensor
 * Powersave: LoRa Sleep & ESP32 Deep Sleep (10 seconds)
 */
 
#include <SPI.h>
#include <LoRa.h>
#include <DHT.h>

// LoRa SPI Pins
#define SCK 5
#define MISO 19
#define MOSI 27
#define ss 18
#define rst 14
#define dio0 26

// DHT Config
#define DHTPIN 4
#define DHTTYPE DHT11 // Change to DHT22 if needed
DHT dht(DHTPIN, DHTTYPE);

// Deep sleep for exactly 10 seconds
#define TIME_TO_SLEEP  10
#define uS_TO_S_FACTOR 1000000ULL

// Store packet count in RTC memory so it survives Deep Sleep!
RTC_DATA_ATTR int packetCount = 0;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  
  Serial.println("\n[P1 SENDER] Waking Up from Deep Sleep...");

  dht.begin();
  
  SPI.begin(SCK, MISO, MOSI, ss);
  LoRa.setPins(ss, rst, dio0);
  
  if (!LoRa.begin(433E6)) {
    Serial.println("[P1 ERROR] LoRa failed to start!");
  } else {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    
    if (isnan(h) || isnan(t)) {
      Serial.println("[P1 ERROR] Failed to read DHT sensor!");
    } else {
      packetCount++; // Increment the packet number
      
      Serial.print("[P1 DATA] Sending #"); 
      Serial.print(packetCount);
      Serial.print(" - Temp: "); 
      Serial.print(t); 
      Serial.print("C, Hum: "); 
      Serial.print(h); 
      Serial.println("%");
      
      // Transmission takes less than ~50ms
      LoRa.beginPacket();
      LoRa.print(packetCount);
      LoRa.print(",");
      LoRa.print(t);
      LoRa.print(",");
      LoRa.print(h);
      LoRa.endPacket();
      
      Serial.print("[P1 SENDER] Packet #");
      Serial.print(packetCount);
      Serial.println(" sent successfully!");
    }
    LoRa.sleep(); // Put LoRa radio to Sleep mode
  }
  
  Serial.println("[P1 SENDER] Going to ESP32 Deep Sleep for 10 seconds...");
  Serial.flush(); 
  
  // Power down ESP32 CPU until 10 seconds is over
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void loop() {
  // Not used because ESP32 shuts down at the end of setup()
}
