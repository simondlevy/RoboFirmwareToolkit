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

static uint16_t speed = 350;

static void setSpeed(uint16_t value)
{
    esc.speed(MIN_SPEED-100+value);
}

void setup()
{
    Serial.begin(115200);

    pinMode(PIN, OUTPUT);

    esc.arm(); // Send the Arm command to ESC

    delay(1000); // Wait a while

    setSpeed(0);

    speed = 0;

    delay(1000); // Wait a while

    speed = 350;
} 

void loop() 
{
    setSpeed(speed);

    delay(10);
}
