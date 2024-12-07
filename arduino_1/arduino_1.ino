//Arduino 1
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>

#define RST_PIN 9
#define SS_PIN 10
#define SLAVE_ADDRESS 8 

MFRC522 mfrc522(SS_PIN, RST_PIN); 

void setup() {
  Serial.begin(9600);          
  SPI.begin();                
  mfrc522.PCD_Init();        
  Wire.begin();               
  delay(4);                   
}

void loop() {
  String cardUID = scanRFIDCard(); 
  if (cardUID.length() > 0) {
    Serial.println("Sending UID to Arduino 2...");
    sendUIDToSlave(cardUID); 
    delay(1000); 
  }
}

String scanRFIDCard() {
  Serial.println("Place your card on the reader...");


  while (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(100);  
  }

  
  String uidString = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }


  mfrc522.PICC_HaltA();

  return uidString;
}

void sendUIDToSlave(String uid) {
  Wire.beginTransmission(SLAVE_ADDRESS);  
  Wire.write(uid.c_str());  
  Wire.endTransmission();  
}