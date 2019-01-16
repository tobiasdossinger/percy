
// Load Wi-Fi library
#include <WiFi.h>

uint8_t motorPin = 12;
uint8_t motorChannel = 1;


// Replace with your network credentials
const char* ssid     = "LocalsOnly";
const char* password = "Tobias!!";

WiFiServer wifiServer(80);


void setup() {
  Serial.begin(115200);
 
  delay(1000);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());

    // Attatch motor pin to channel
  ledcAttachPin(motorPin, motorChannel);
  // Initialize motor channel with 12 kHz PWM frequency and 8-bit resolution
  ledcSetup(motorChannel, 12000, 8);
 
  wifiServer.begin();
}

void loop(){

  WiFiClient client = wifiServer.available();
 
  if (client) {
 
    while (client.connected()) {

      String message = "";
 
      while (client.available()>0) {
        char c = client.read();
        message = String(message + c);
      }

      if(message != "") {
        Serial.println(message);
        ledcWrite(motorChannel, message.toInt());
      }
      
 
      delay(10);
    }
 
    client.stop();
    Serial.println("Client disconnected");
 
  }

}
