/*
 * Adapted from https://esp32.com/viewtopic.php?t=20450
 */


#include <ESP32Servo.h> // ESP32Servo library installed by Library Manager
#include "ESC.h" // RC_ESP library installed by Library Manager

static const uint8_t PIN = 25; // connected to ESC control wire

// Note: the following speeds may need to be modified for your particular hardware.
static const uint16_t MIN_SPEED = 1040; // speed just slow enough to turn motor off
static const uint16_t MAX_SPEED = 1240; // speed where my motor drew 3.6 amps at 12v.

static ESC esc (PIN, 1000, 2000, 500); // ESC_Name (PIN, Minimum Value, Maximum Value, Arm Value)

void setup()
{
    Serial.begin(115200);

    pinMode(PIN, OUTPUT);

    esc.arm(); // Send the Arm command to ESC

    delay(1000); // Wait a while
} 

void loop() 
{
    // the following loop turns on the motor slowly, so get ready
    for (int i=0; i<700; i++){ // run speed from 840 to 1190
        uint16_t speed = MIN_SPEED-100+i; // motor starts up about half way through loop
        esc.speed(speed);
        delay(10);
    }
}
