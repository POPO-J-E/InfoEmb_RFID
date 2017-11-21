// Librairie de gestion RFID
#include "MFRC522.h"


// Broche commandant le relai
#define RELAY 5

//=== https://github.com/esp8266/Arduino/tree/master/libraries/EEPROM/examples 
#include <EEPROM.h>
//This is a bit different from standard EEPROM class. 
//You need to call EEPROM.begin(size) before you start reading or writing, 
//size being the number of bytes you want to use. Size can be anywhere between 4 and 4096 bytes.
//EEPROM.write does not write to flash immediately, instead you must call 
//EEPROM.commit() whenever you wish to save changes to flash. EEPROM.end() will also commit, 
//and will release the RAM copy of EEPROM contents.
//EEPROM library uses one sector of flash located just after the SPIFFS.

// Copyright Benoit Blanchon 2014-2017 // MIT License // Arduino JSON library
// https://bblanchon.github.io/ArduinoJson/
#include <ArduinoJson.h>




 

// DÃ©finition de la broche SS (Select) qui va Ãªtre utilisÃ© pour sÃ©lectionner
// Le chip -RC522
#define SS_PIN  15 
//------ Broche pilotant le reset du chip RC522
#define RST_PIN  16 // RST-PIN for RC522 - RFID - SPI - Modul GPIO5 

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance



void setup() 
{
Serial.begin(9600);    // Initialize serial communications
EEPROM.begin(512);

SPI.begin();           // Init SPI bus
mfrc522.PCD_Init();    // Init MFRC522
Serial.println ("Serial init .. PASS");
// Configuration PIN de commande du relai
pinMode(RELAY, OUTPUT);
digitalWrite(RELAY, LOW);


Serial.print("EEprom[0]= ");
Serial.println(EEPROM.read(0));

// ATTENTION : Le nombre d'Ã©criture en EEPROM est limitÃ© env 500K Ã  1000 K 
//Serial.print("Try to write 0x55");
//EEPROM.write(0, 0x55);
//EEPROM.commit();
//Serial.print("EEprom[0]= ");
//Serial.println(EEPROM.read(0));

}


void loop() { 
 StaticJsonBuffer<300> jsonBuffer;
  // Look for new cards
  //Serial.print(".");
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    delay(50);
    return;
  }
  
  
  // Select one of the cards
  Serial.println("card found ...");
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    
    delay(50);
    return;
  }
  
  // Show some details of the PICC (that is: the tag/card)
  Serial.print(F("Card UID:"));
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();
  digitalWrite(RELAY, HIGH);
  
  // remplissage de la chaine json
  JsonObject& root = jsonBuffer.createObject();
  root["MSG"] = 1;                    // Type de trame JSON
  root["SRC"] = 2;
  root["GW_RSSI"] = 3;
  JsonArray& data = root.createNestedArray("UID");
  for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
    data.add(mfrc522.uid.uidByte[i]);
    }
  root.printTo(Serial);
  Serial.println("");


  delay(500);
  digitalWrite(RELAY, LOW);
  delay(100);
}



// Helper routine to dump a byte array as hex values to Serial
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
delay(100);
}



