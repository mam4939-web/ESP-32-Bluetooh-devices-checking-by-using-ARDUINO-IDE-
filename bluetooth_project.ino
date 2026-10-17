#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int scanTime = 5; 
BLEScan* pBLEScan;

int totalFound = 0;
String lastMAC = "Searching...";
int lastRSSI = 0;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      totalFound++;
      lastMAC = advertisedDevice.getAddress().toString().c_str();
      lastRSSI = advertisedDevice.getRSSI();

      Serial.print("[BLE] MAC: ");
      Serial.print(lastMAC);
      Serial.print(" | RSSI: ");
      Serial.println(lastRSSI);

      drawScreen("SCANNING...", totalFound, lastMAC, lastRSSI);
    }

    void drawScreen(String status, int count, String mac, int rssi) {
      display.clearDisplay();
      display.setTextColor(SSD1306_WHITE);
      
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println("=== BLE AUDITOR ===");
      
      display.setCursor(0, 14);
      display.print("Status: ");
      display.println(status);
      
      display.setCursor(0, 26);
      display.print("Found: ");
      display.setTextSize(2); 
      display.setCursor(50, 24);
      display.println(count);
      
      display.setTextSize(1);
      display.setCursor(0, 44);
      display.print("Last MAC: ");
      display.println(mac.substring(0, 12));
      
      display.setCursor(0, 55);
      display.print("RSSI: ");
      display.print(rssi);
      display.println(" dBm");
      
      display.display();
    }
};

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(21, 22);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("ОШИБКА: OLED экран не найден! Проверьте провода (GND, VCC, SDA, SCL).");
    for(;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 20);
  display.println("BLE Scanner");
  display.setCursor(15, 35);
  display.println("Initializing...");
  display.display();
  delay(2000);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); 
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);   
  pBLEScan->setInterval(130);      
  pBLEScan->setWindow(99);         
}

void loop() {
  totalFound = 0;
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("=== BLE AUDITOR ===");
  display.setCursor(0, 25);
  display.println("Starting scan...");
  display.display();

  pBLEScan->start(scanTime, false);
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("=== SCAN COMPLETE ===");
  display.setCursor(0, 20);
  display.print("Total Devices: ");
  display.println(totalFound);
  display.setCursor(0, 40);
  display.println("Pause 3 sec...");
  display.display();

  pBLEScan->clearResults(); 
  delay(3000); 
}