// Define PIN and Channel for the motor
uint8_t motorPin = 12;
uint8_t motorChannel = 1;


void setup() {
    // Attatch motor pin to channel
    ledcAttachPin(motorPin, motorChannel);
    // Initialize motor channel with 12 kHz PWM frequency and 8-bit resolution
    ledcSetup(motorChannel, 12000, 8);
}


void loop() {
    // Count up from 0 to 255
    for (int val = 0; val < 255; val++) {
        // Write value (0-255) of the loop to the motor channel
        ledcWrite(motorChannel, val);
        // Wait a bit
        delay(100);
    }
}
