
#include <WiFi.h>
#include <HttpClient.h>
#include <WString.h>

char ssid[] = "dlink1"; //  your network SSID (name) 
char pass[] = "ceciestunmotdepasse";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "192.168.0.1";    // name address for Google (using DNS)
char uid[] = "toto";
char pss[] = "titi";

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
WiFiClient wifiClient;
HttpClient client(wifiClient);
String readString = "";

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  setupWifi();
}

void loop() {
  char test[] = "test";
  checkAccess(test);
}

boolean checkAccess(char rfid[])
{
  char address[100];
  sprintf(address, "%s/?uid=%s&pss=%s&rfid=%s", server, uid, pss, rfid);
  
  // Make a HTTP request: 
  client.get(server, address);
  
  // if there are incoming bytes available 
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();
    
    readString += c;
  }

  boolean allowed = (readString.indexOf("ACCESS ALLOWED") > 0);
  readString = "";
  return allowed;
}

void setupWifi() {
  delay(10);
  
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    // don't continue:
    while(true);
  } 
  
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid, pass);
  
    // wait 10 seconds for connection:
    delay(10000);
  } 
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
