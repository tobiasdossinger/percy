// Load Wi-Fi library
#include <WiFi.h>

  // Replace with your network credentials
  const char * ssid = "LocalsOnly";
const char * password = "Tobias!!";

WiFiServer wifiServer(80);

struct activePattern {
  int tStart;
  int tEnd;
  int tDuration;
  int motorID;
  int patternID;
};

int motorPINs[] = {12, 14, 27, 26};
activePattern activePatterns[sizeof(motorPINs) / sizeof(motorPINs[0])];

float patternList[3][5][2] = {
  {
    {0, 0},
    {0.5, 0.25},
    {0.75, 1},
    {1, 0}
  },
  {
    {0, 0},
    {0.25, 0.5},
    {0.5, 0},
    {0.75, 1},
    {1, 0}
  },
  {
    {0, 0},
    {0.4, 1},
    {0.6, 0},
    {0.8, 1},
    {1, 0}
  }
};

void addPattern(int motorID, int patternID, int duration) {
  int tStart = millis();
  activePattern testPattern = {
    tStart,
    tStart + duration,
    duration,
    motorID,
    patternID
  };

  Serial.println("Added pattern");

  activePatterns[motorID] = testPattern;
}

int messagePart = 0;
String motorID = "";
String patternID = "";
String motorPower = "";

int tCurrent;


void socketLoop(char c) {
  if (c == '-') {
    messagePart++;
  } else if (c == 'x') {
    Serial.println("Motor: " + motorID + " - Pattern: " + patternID + " - Duration: " + motorPower);
    addPattern(motorID.toInt(), patternID.toInt(), motorPower.toInt());
    motorID = "";
    patternID = "";
    motorPower = "";
    messagePart = 0;
  } else if (messagePart == 0) {
    motorID = motorID + c;
  } else if (messagePart == 1) {
    patternID = patternID + c;
  } else if (messagePart == 2) {
    motorPower = motorPower + c;
  }

  delay(10);
}

void motorLoop() {
  for (int i = 0; i < sizeof(motorPINs) / sizeof(motorPINs[0]); i++) {
  
    activePattern currentPattern = activePatterns[i];

    if (tCurrent < currentPattern.tEnd) {
      float totalProgress = float(tCurrent - currentPattern.tStart) / float(currentPattern.tDuration);

      int topIndex = 0;

      for (int ii = 0; ii < sizeof(patternList[currentPattern.patternID]) / sizeof(patternList[currentPattern.patternID][0]); ii++) {
        if (patternList[currentPattern.patternID][ii][0] > totalProgress) {
          topIndex = ii;
          break;
        }
      }

      float * sectionStart = patternList[currentPattern.patternID][topIndex - 1];
      float * sectionEnd = patternList[currentPattern.patternID][topIndex];

      float sectionProgress = (totalProgress - sectionStart[0]) / (sectionEnd[0] - sectionStart[0]);
      float currentIntensity = sectionStart[1] + ((sectionEnd[1] - sectionStart[1]) * sectionProgress);
      ledcWrite(i + 1, 255 * currentIntensity);

            Serial.print("IDs: ");
            Serial.print(i);
            Serial.print(" | ");
            Serial.print(topIndex);
            Serial.print(" --- ");
            
            Serial.print("Progress: ");
            Serial.print(totalProgress);
            Serial.print(" / ");
            Serial.print(sectionProgress);
            Serial.print(" (");
            Serial.print(sectionStart[0]);
            Serial.print(" | ");
            Serial.print(sectionEnd[0]);
            Serial.print(") --- ");
            
            Serial.print("Intensity: ");
            Serial.print(currentIntensity);
            Serial.print(" - ");
            Serial.print(255 * currentIntensity);
            Serial.print(" (");
            Serial.print(sectionStart[1]);
            Serial.print(" | ");
            Serial.print(sectionEnd[1]);
            Serial.print(") --- ");
            
            Serial.println(tCurrent);

    } else {
      ledcWrite(i + 1, 0);
    }
  }

  tCurrent = millis();
}

void setup() {
  Serial.begin(9600);

  delay(1000);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());

  for (int i = 0; i < sizeof(motorPINs) / sizeof(motorPINs[0]); i++) {
    // Attatch motor pin to channel
    ledcAttachPin(motorPINs[i], i + 1);
    // Initialize motor channel with 12 kHz PWM frequency and 8-bit resolution
    ledcSetup(i + 1, 12000, 8);
  }

  tCurrent = millis();

  wifiServer.begin();
}

void loop() {

  WiFiClient client = wifiServer.available();
  motorLoop();

  if (client) {
    while (client.connected()) {
      motorLoop();

      while (client.available() > 0) {
        char c = client.read();
        socketLoop(c);
      }
    }

    client.stop();
    Serial.println("Client disconnected");

  }
}
