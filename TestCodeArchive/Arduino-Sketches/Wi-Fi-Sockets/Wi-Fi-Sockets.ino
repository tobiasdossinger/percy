// Load Wi-Fi library
#include <WiFi.h>

uint8_t motorPin1 = 12;
uint8_t motorPin2 = 14;
uint8_t motorPin3 = 27;
uint8_t motorPin4 = 26;


uint8_t motorChannel1 = 1;
uint8_t motorChannel2 = 2;
uint8_t motorChannel3 = 3;
uint8_t motorChannel4 = 4;


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
  ledcAttachPin(motorPin1, motorChannel1);
  ledcAttachPin(motorPin2, motorChannel2);
  ledcAttachPin(motorPin3, motorChannel3);
  ledcAttachPin(motorPin4, motorChannel4);
  
  // Initialize motor channel with 12 kHz PWM frequency and 8-bit resolution
  ledcSetup(motorChannel1, 12000, 8);
  ledcSetup(motorChannel2, 12000, 8);
  ledcSetup(motorChannel3, 12000, 8);
  ledcSetup(motorChannel4, 12000, 8);
 
  wifiServer.begin();
}

void loop(){

  WiFiClient client = wifiServer.available();
 
  if (client) {
 
    while (client.connected()) {

      int messagePart = 0;
      String motorID = "";
      String motorPower = "";

      while (client.available()>0) {

        char c = client.read();
        if(c == '-') {
          messagePart++;
        } else if(c == 'x') {
          Serial.print("Motor: ");
          Serial.print(motorID);
          Serial.print(" - Power: ");
          Serial.println(motorPower);
          ledcWrite(motorID.toInt(), motorPower.toInt());
          motorID = "";
          motorPower = "";
          messagePart = 0;
        } else if(messagePart == 0) {
           motorID = motorID + c;
        } else if(messagePart == 1) {
          motorPower = motorPower + c;
        }
      }
      delay(10);
    }
 
    client.stop();
    Serial.println("Client disconnected");
 
  }

}
