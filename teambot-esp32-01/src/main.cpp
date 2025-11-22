#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <PubSubClient.h>

// Define pins for MFRC522
#define SS_PIN 5    // SDA/SS pin
#define RST_PIN 22  // Reset pin

// Function declaration
void sendToServer(String rfidData);
void reconnectMQTT();

// WiFi credentials
const char* ssid = "Testing Area";
const char* password = "Kyukken2020!";

// PHP server URL
const char* serverUrl = "http://10.133.214.124/rfid_handler.php";

// MQTT broker details
const char* mqttServer = "10.133.214.124";
const int mqttPort = 1883;
const char* mqttTopic = "RFID_LOGIN";

// Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

// MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

// MQTT status tracking
bool mqttConnected = false;

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
  
      // Setup MQTT
      client.setServer(mqttServer, mqttPort);
      reconnectMQTT();
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

  // Check MQTT connection
    if (!client.connected()) {
      if (mqttConnected) {
        Serial.println("MQTT connection lost!");
        mqttConnected = false;
      }
      reconnectMQTT();
    }
    client.loop();
  
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

void reconnectMQTT() {
  while (!client.connected()) {
    if (!mqttConnected) {
      Serial.print("Attempting MQTT connection...");
    }
    if (client.connect("TeamBot-esp32-01")) {
      if (!mqttConnected) {
        Serial.println("connected");
        mqttConnected = true;
      }
    } else {
      if (mqttConnected) {
        Serial.println("MQTT disconnected!");
        mqttConnected = false;
      }
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
  if (!mqttConnected) {
    Serial.println("MQTT reconnected");
    mqttConnected = true;
  }
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
        // Publish rfid_status to MQTT
        if (client.connected()) {
          client.publish(mqttTopic, "-1");
          Serial.println("Published -1 to MQTT");
        } else {
          Serial.println("MQTT not connected, cannot publish");
        }
      } else if (response.indexOf("STATUS:") != -1) {
        // Extract status from response
        int statusIndex = response.indexOf("STATUS:");
        if (statusIndex != -1) {
          String status = response.substring(statusIndex + 7, statusIndex + 8);
          Serial.print("Status: ");
          Serial.println(status);
          // Publish rfid_status to MQTT
          if (client.connected()) {
            client.publish(mqttTopic, status.c_str());
            Serial.println("Published status to MQTT");
          } else {
            Serial.println("MQTT not connected, cannot publish");
          }
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