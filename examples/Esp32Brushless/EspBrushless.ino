/*
 * Adapted from https://esp32.com/viewtopic.php?t=20450
 */


#include <ESP32Servo.h> // ESP32Servo library installed by Library Manager
#include "ESC.h" // RC_ESP library installed by Library Manager

static uint8_t ESC_PIN = 25; // connected to ESC control wire

// Note: the following speeds may need to be modified for your particular hardware.
#define MIN_SPEED 1040 // speed just slow enough to turn motor off
#define MAX_SPEED 1240 // speed where my motor drew 3.6 amps at 12v.

ESC myESC (ESC_PIN, 1000, 2000, 500); // ESC_Name (PIN, Minimum Value, Maximum Value, Arm Value)

long int val; // variable to read the value from the analog pin

void setup()
{
    pinMode(ESC_PIN, OUTPUT);

    myESC.arm(); // Send the Arm command to ESC

    delay(1000); // Wait a while


} // speed will now jump to pot setting

void loop() 
{
    // the following loop turns on the motor slowly, so get ready
    for (int i=0; i<350; i++){ // run speed from 840 to 1190
        myESC.speed(MIN_SPEED-200+i); // motor starts up about half way through loop
        delay(10);
    }

}
