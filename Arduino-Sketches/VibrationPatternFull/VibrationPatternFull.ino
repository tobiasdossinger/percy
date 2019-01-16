// From https://github.com/ivanseidel/LinkedList
#include <LinkedList.h>

struct activePattern {
  int tStart;
  int tEnd;
  int tDuration;
  int repeat;
  int motorID;
  int patternID;
};

LinkedList<activePattern> activePatterns;

static const int motorPINs[3] = {7, 8, 9};

static const float patternList[3][5][2] = {
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

void addPattern(int duration, int repeat, int motorID, int patternID) {
  int tStart = millis();
  activePattern testPattern = {
    tStart,
    tStart + duration,
    duration,
    repeat,
    motorID,
    patternID
  };

  activePatterns.add(testPattern);
}

int tCurrent;


void setup() {
  Serial.begin(9600);
  addPattern(2000, 1, 0, 2);
  
  // Attatch motor pin to channel
  ledcAttachPin(7, 1);
  // Initialize motor channel with 12 kHz PWM frequency and 8-bit resolution
  ledcSetup(1, 12000, 8);

  tCurrent = millis();
  pinMode(7, OUTPUT);
  Serial.println("=======================");
  Serial.println(activePatterns.size());
  Serial.println("=======================");
}

void loop() {
  int patternCount = activePatterns.size();
  
  if(patternCount) {
    for(int i = 0; i < patternCount; i++) {
      activePattern currentPattern = activePatterns.get(i);
  
      if(tCurrent < currentPattern.tEnd) {
        float totalProgress = float(tCurrent - currentPattern.tStart) / float(currentPattern.tDuration);

        int ii = 0;

        for(ii; ii < sizeof(patternList[currentPattern.patternID]) / sizeof(patternList[currentPattern.patternID][0]); ii++) {
          if(patternList[currentPattern.patternID][ii][0] > totalProgress) {
            break;
          }
        }

        float *sectionStart = patternList[currentPattern.patternID][ii - 1];
        float *sectionEnd = patternList[currentPattern.patternID][ii];

        float sectionProgress = (totalProgress - sectionStart[0]) / (sectionEnd[0] - sectionStart[0]);
        float currentIntensity = sectionStart[1] + ((sectionEnd[1] - sectionStart[1]) * sectionProgress);
        ledcWrite(1, 255 * currentIntensity);
        // analogWrite(motorPINs[currentPattern.motorID], 255 * currentIntensity);

        // Debug print code can be put here
        
      } else {
        ledcWrite(1, 0);
        // analogWrite(motorPINs[currentPattern.motorID], 0);
        activePatterns.remove(i);
      }
    }
  }

  tCurrent = millis();
}





// Debug Print Code

//        Serial.print("IDs: ");
//        Serial.print(i);
//        Serial.print(" | ");
//        Serial.print(ii);
//        Serial.print(" --- ");
//        
//        Serial.print("Progress: ");
//        Serial.print(totalProgress);
//        Serial.print(" / ");
//        Serial.print(sectionProgress);
//        Serial.print(" (");
//        Serial.print(sectionStart[0]);
//        Serial.print(" | ");
//        Serial.print(sectionEnd[0]);
//        Serial.print(") --- ");
//        
//        Serial.print("Intensity: ");
//        Serial.print(currentIntensity);
//        Serial.print(" - ");
//        Serial.print(255 * currentIntensity);
//        Serial.print(" (");
//        Serial.print(sectionStart[1]);
//        Serial.print(" | ");
//        Serial.print(sectionEnd[1]);
//        Serial.print(") --- ");
//        
//        Serial.println(tCurrent);
