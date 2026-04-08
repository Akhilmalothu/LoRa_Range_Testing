/*
 * === P2 (YOU) RECEIVER NODE ===
 * Components: ESP32 + LoRa SX1278 + I2C LCD Display
 * Powersave: Synchronized Listening window (11s) + ESP32 Deep Sleep (10s)
 */

#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// LoRa SPI Pins
#define SCK 5
#define MISO 19
#define MOSI 27
#define ss 18
#define rst 14
#define dio0 26

// P1 sleeps for 10s. P2 listens for 11s to guarantee catching P1!
#define TIME_TO_SLEEP  10
#define uS_TO_S_FACTOR 1000000ULL
#define LISTEN_WINDOW_MS 11000 

void setup() {
  Serial.begin(115200);
  while(!Serial);

  Serial.println("\n[P2 RECEIVER] Waking Up from Deep Sleep...");

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0); lcd.print("LoRa Receiver");
  lcd.setCursor(0, 1); lcd.print("Listening...");

  SPI.begin(SCK, MISO, MOSI, ss);
  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) {
    Serial.println("[P2 ERROR] LoRa failed to start!");
    lcd.setCursor(0, 1);
    lcd.print("LoRa Fail!");
    while (1);
  }
  
  Serial.println("[P2 RECEIVER] Listening for 11 seconds to catch P1's packet...");

  unsigned long startListenTime = millis();
  bool packetReceived = false;

  // Active Listening Sync Window
  while (millis() - startListenTime < LISTEN_WINDOW_MS) {
    if (LoRa.parsePacket()) {
      Serial.println("\n[P2 RECEIVER] Caught P1's Transmission!");
      
      String receivedText = "";
      while (LoRa.available()) {
        receivedText += (char)LoRa.read();
      }
      
      Serial.print("[P2 DATA] Received: '");
      Serial.print(receivedText);
      Serial.println("'");
      
      // Update the LCD screen
      int firstComma = receivedText.indexOf(',');
      int secondComma = receivedText.indexOf(',', firstComma + 1);
      
      if(firstComma > 0 && secondComma > firstComma) {
        String pCountStr = receivedText.substring(0, firstComma);
        String t = receivedText.substring(firstComma + 1, secondComma);
        String h = receivedText.substring(secondComma + 1);
        
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("#"); lcd.print(pCountStr); lcd.print(" T:"); lcd.print(t); lcd.print("C");
        lcd.setCursor(0, 1); lcd.print("Hum: "); lcd.print(h); lcd.print("%");
      }
      
      packetReceived = true;
      break;  // Abort the 11-second listening window immediately to save power!
    }
  }

  if (!packetReceived) {
    Serial.println("\n[P2 ERROR] Missed P1's transmission this time.");
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("Missed Packet!");
  } else {
    // Leave the LCD screen on for 2 seconds so you can read it before it goes black
    delay(2000); 
  }

  Serial.println("\n[P2 RECEIVER] Putting LoRa radio to Sleep mode...");
  LoRa.sleep();

  Serial.println("[P2 RECEIVER] Going to ESP32 Deep Sleep for 10 seconds...");
  Serial.flush();
  
  // Power down ESP32 CPU until 10 seconds is over
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void loop() {
  // Not used because ESP32 shuts down at the end of setup()
}
