#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Define pins for MFRC522
#define SS_PIN 5    // SDA/SS pin
#define RST_PIN 22  // Reset pin

// Function declaration
void sendToServer(String rfidData);

// WiFi credentials
const char* ssid = "Testing Area";
const char* password = "Kyukken2020!";

// PHP server URL (UPDATE THIS WITH YOUR SERVER URL)
const char* serverUrl = "http://192.168.1.7/rfid_handler.php";

// Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println("\n=================================");
  Serial.println("ESP32 RFID System Starting...");
  Serial.println("=================================\n");
  
  // Initialize SPI bus
  SPI.begin();
  
  // Initialize MFRC522
  mfrc522.PCD_Init();
  Serial.println("RFID Reader Initialized");
  
  // Connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi Connection Failed!");
  }
  
  Serial.println("\nReady to scan RFID cards...\n");
}

void loop() {
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.begin(ssid, password);
    delay(5000);
    return;
  }
  
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  // Get card UID
  String cardID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    cardID += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    cardID += String(mfrc522.uid.uidByte[i], HEX);
  }
  cardID.toUpperCase();
  
  Serial.print("Card Scanned: ");
  Serial.println(cardID);
  
  // Send data to server
  sendToServer(cardID);
  
  // Halt PICC
  mfrc522.PICC_HaltA();
  
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
  
  // Delay before next read
  delay(2000);
}

void sendToServer(String rfidData) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Prepare URL with query parameter
    String url = String(serverUrl) + "?rfid=" + rfidData;
    
    Serial.print("Sending request to: ");
    Serial.println(url);
    
    // Begin HTTP connection
    http.begin(url);
    
    // Send GET request
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("Server Response: ");
      Serial.println(response);
      
      // Parse response
      if (response.indexOf("NOT FOUND") != -1) {
        Serial.println("Status: RFID NOT FOUND");
      } else if (response.indexOf("STATUS:") != -1) {
        // Extract status from response
        int statusIndex = response.indexOf("STATUS:");
        if (statusIndex != -1) {
          String status = response.substring(statusIndex + 7, statusIndex + 8);
          Serial.print("Status: ");
          Serial.println(status);
        }
      }
    } else {
      Serial.print("Error sending request. HTTP Code: ");
      Serial.println(httpResponseCode);
    }
    
    // End HTTP connection
    http.end();
  } else {
    Serial.println("WiFi not connected. Cannot send data.");
  }
}