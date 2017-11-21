#include <ESP8266HTTPClient.h>

#include <b64.h>

#include <ESP8266WiFi.h>
//=== https://github.com/amcewen/HttpClient
#include <WString.h>


// Librairie de gestion RFID
#include "MFRC522.h"
// https://bblanchon.github.io/ArduinoJson/
#include <ArduinoJson.h>
//=== https://github.com/esp8266/Arduino/tree/master/libraries/EEPROM/examples 
#include <EEPROM.h>
// Broche commandant le relai
#define RELAY 5
// DÃ©finition de la broche SS (Select) qui va Ãªtre utilisÃ© pour sÃ©lectionner
// Le chip -RC522
#define SS_PIN  15 
//------ Broche pilotant le reset du chip RC522
#define RST_PIN  16 // RST-PIN for RC522 - RFID - SPI - Modul GPIO5 

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

char ssid[] = "linksys_1"; //  your network SSID (name) 
char pass[] = "ceciestunmotdepasse";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char host[] = "192.168.20.117";    // name address for Google (using DNS)
int port = 9191;
char uid[] = "toto";
char pss[] = "titi";

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
WiFiClient wifiClient;
HTTPClient  http;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600); 
  Serial.println("Begin !");
  delay(1);
  //while (!Serial) {
    //; // wait for serial port to connect. Needed for Leonardo only
  //}
  
  setupWifi();
  setupRFIDLoader();
}

void loop() {

  loopRFID();
}

void loopRFID()
{
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

  String rfid = getRFID(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.print(F("Card UID:"));
  Serial.println(rfid);

  if(checkAccess(rfid))
  {
    Serial.println("Access allowed");
    openDoor();
  }
  else
  {
    Serial.println("Access not allowed");
    errorDoor();
  }
}

void openDoor()
{
  Serial.println("Open door");
  digitalWrite(RELAY, HIGH);
  delay(3000);
  Serial.println("Close door");
  digitalWrite(RELAY, LOW);
  delay(100);
}


void errorDoor()
{
  Serial.println("Error door");
  digitalWrite(RELAY, HIGH);
  delay(100);
  digitalWrite(RELAY, LOW);
  delay(100);
  digitalWrite(RELAY, HIGH);
  delay(100);
  digitalWrite(RELAY, LOW);
  delay(100);
  digitalWrite(RELAY, HIGH);
  delay(100);
  digitalWrite(RELAY, LOW);
  delay(100);
}

boolean checkAccess(String rfid)
{
  String url = "/?uid=";
      url += uid;
      url += "&pss=";
      url += pss;
      url += "&rfid=";
      url += rfid;
      
  Serial.print("connecting to ");
  Serial.println(host);
  Serial.print("on port ");
  Serial.println(port);
  Serial.print("Requesting URL: ");
  Serial.println(url);

  String payload = "";
  
  http.begin(host,port,url);
  int httpCode = http.GET();
  if (httpCode) {
    if (httpCode == 200) {
      payload = http.getString();
    }
  }
  Serial.println("closing connection");
  http.end();
  
  boolean allowed = (payload.indexOf("ACCESS ALLOWED") >= 0);
  return allowed;
}

void setupRFIDLoader()
{
  EEPROM.begin(512);

  SPI.begin();           // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522
  Serial.println ("Serial init .. PASS");
  // Configuration PIN de commande du relai
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);
  
  
  Serial.print("EEprom[0]= ");
  Serial.println(EEPROM.read(0));
}

void setupWifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    //extButton();
    for(int i = 0; i<500; i++){
      //extButton();
      delay(1);
    }
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  Serial.println("Connected to wifi");
  printWifiStatus();
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

// Helper routine to dump a byte array as hex values to Serial
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
delay(100);
}

String getRFID(byte *buffer, byte bufferSize) {
  String rfid = "";
  for (byte i = 0; i < bufferSize; i++) {
    if(i != 0)
    {
      rfid += buffer[i] < 0x10 ? ".0" : ".";
    }
    rfid += buffer[i], DEC;
  }
  return rfid;
}

