
// Load Wi-Fi library
#include <WiFi.h>



// Replace with your network credentials
const char* ssid     = "LocalsOnly";
const char* password = "Tobias!!";


// Define PIN and Channel for the motor
uint8_t motorPin = 12;

uint8_t motorChannel = 1;

// Variable to store the HTTP request
String header;


// Auxiliar variables to store the current output state
String motorPinStateStop = "off";
String motorPinStateRechts = "off";
String motorPinStateLinks = "off";



// Set web server port number to 80
WiFiServer server(80);


void setup() {
  Serial.begin(115200);
  
  // Attatch motor pin to channel
  ledcAttachPin(motorPin, motorChannel);
  // Initialize motor channel with 12 kHz PWM frequency and 8-bit resolution
  ledcSetup(motorChannel, 12000, 8);

  
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();




/*------------------------------- Turn Stuff (Pattern) on and off  -----------------------------------------*/
     

            // Center Motor - Stop - Glasses
            if (header.indexOf("GET /12/onMuster1") >= 0) {
              Serial.println("GPIO 12 onMuster1");
              motorPinStateStop = "on";
              ledcWrite(motorChannel, 255);

            } else if (header.indexOf("GET /12/offMuster1") >= 0) {
              Serial.println("GPIO 12 offMuster1");
              motorPinStateStop = "off";
              ledcWrite(motorChannel, 0);


            } 
            // Right Motor - Right Shoulder
            else if (header.indexOf("GET /12/onMuster2") >= 0) {
              Serial.println("GPIO 12 onMuster2");
              motorPinStateRechts = "on";
              ledcWrite(motorChannel, 60);
              delay(500);
              ledcWrite(motorChannel, 100);
              delay(500);
              ledcWrite(motorChannel, 100);  
            } 

            else if (header.indexOf("GET /12/offMuster2") >= 0) {
              Serial.println("GPIO 12 offMuster2");
              motorPinStateRechts = "off";
              ledcWrite(motorChannel, 0);
            }


            // Left Motor - Left Shoulder
             else if (header.indexOf("GET /12/onMuster3") >= 0) {
              Serial.println("GPIO 12 onMuster3");
              motorPinStateLinks = "on";

              //second parameter = val(pattern)
              ledcWrite(motorChannel, 150);

              // Count up from 0 to 255
//              for (int val = 0; val < 255; val++) {
//                  // Write value (0-255) of the loop to the motor channel
//                  ledcWrite(motorChannel, val);
//                  // Wait a bit
//                  delay(100);
//              }
              
            } 
            
            else if (header.indexOf("GET /12/offMuster3") >= 0) {
              Serial.println("GPIO 12 offMuster3");
              motorPinStateLinks = "off";
              ledcWrite(motorChannel, 0);
            }



            
/*--------------------------------------- HTML & CSS Implementation ---------------------------------------------- */       

            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");



            /* CSS Button Classes */
            
            // CSS to style the on/off buttons 
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");

            // Stop Button
            client.println(".buttonStopMusterOn {background-color: #c32439; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".buttonStopMusterOff {background-color: #24c36b;}");
            
            
            //Right Button
            client.println(".buttonRechtsMusterOn {background-color: #d2ddd8; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".buttonRechtsMusterOff {background-color: #24c36b;}");

            
            //Left Button
            client.println(".buttonLinksMusterOn {background-color: #d2ddd8; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".buttonLinksMusterOff {background-color: #24c36b;}  </style></head>");
            
            


            // Web Page Heading
            client.println("<body><h1>Percy Vibrationsmotoren Test</h1>");
            
            // Rechter Vibrationsmotor in HTML  
            client.println("<p>Stop Vibrationsmuster " + motorPinStateStop + "</p>");
            // If the motorPinState is off, it displays the ON button       
            if (motorPinStateStop=="off") {
              client.println("<p><a href=\"/12/onMuster1\"><button class=\"buttonStopMusterOn\">STOP</button></a></p>");
            } else {
              client.println("<p><a href=\"/12/offMuster1\"><button class=\"buttonStopMusterOn buttonStopMusterOff\">GO</button></a></p>");

            } 

              // Rechter Vibrationsmotor in HTML
            client.println("<p>Rechter Vibrationsmotor " + motorPinStateRechts + "</p>");
            // If the motorPinState is off, it displays the ON button       
            if (motorPinStateRechts=="off") {
              client.println("<p><a href=\"/12/onMuster2\"><button class=\"buttonRechtsMusterOn\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/12/offMuster2\"><button class=\"buttonRechtsMusterOn buttonRechtsMusterOff\">ON</button></a></p>");
            } 

            client.println("<p>Linker Vibrationsmotor " + motorPinStateLinks + "</p>");
            // If the motorPinState is off, it displays the ON button       
            if (motorPinStateLinks=="off") {
              client.println("<p><a href=\"/12/onMuster3\"><button class=\"buttonLinksMusterOn\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/12/offMuster3\"><button class=\"buttonLinksMusterOn buttonLinksMusterOff\">ON</button></a></p>");
            } 
 
            
            
            


            
            client.println("</body></html>");
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
    
  }
}
