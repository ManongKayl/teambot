#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

// Define pins for MFRC522
#define SS_PIN 5    // SDA/SS pin
#define RST_PIN 22  // Reset pin

// Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial);
  
  // Initialize SPI bus
  SPI.begin();
  
  // Initialize MFRC522
  mfrc522.PCD_Init();
  
  Serial.println("RFID Reader Ready");
}

void loop() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  // Display card UID in one line
  String cardID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    cardID += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    cardID += String(mfrc522.uid.uidByte[i], HEX);
  }
  cardID.toUpperCase();
  
  Serial.println(cardID);
  
  // Halt PICC
  mfrc522.PICC_HaltA();
  
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
  
  // Small delay before next read
  delay(1000);
}