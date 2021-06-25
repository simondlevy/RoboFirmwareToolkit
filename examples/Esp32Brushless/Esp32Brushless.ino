/*
 * Adapted from https://esp32.com/viewtopic.php?t=20450
 */


#include <DSMRX.h>

#include <ESP32Servo.h> // ESP32Servo library installed by Library Manager
#include "ESC.h" // RC_ESP library installed by Library Manager

static const uint8_t CHANNELS = 8;

DSM2048 rx;

static const uint8_t ESC_PIN = 25; // connected to ESC control wire

// Note: the following speeds may need to be modified for your particular hardware.
static const uint16_t MIN_SPEED = 1040; // speed just slow enough to turn motor off
static const uint16_t MAX_SPEED = 1240; // speed where my motor drew 3.6 amps at 12v.

static ESC esc (ESC_PIN, 1000, 2000, 500); // ESC_Name (ESC_PIN, Minimum Value, Maximum Value, Arm Value)

static void rxTask(void * params)
{
    while (true) {
      
        if (Serial1.available()) {
           rx.handleSerialEvent(Serial1.read(), micros()); 
        }

        delay(1);
    } 
}

static void setSpeed(float value)
{
    esc.speed(MIN_SPEED - 100 + (uint16_t)(350 * value));
}

void setup()
{
    Serial.begin(115200);

    Serial1.begin(115000, SERIAL_8N1, 32, 33);

    TaskHandle_t task;
    xTaskCreatePinnedToCore(rxTask, "Task", 10000, NULL, 1, &task, 0); 

    pinMode(ESC_PIN, OUTPUT);

    esc.arm(); // Send the Arm command to ESC

    delay(1000); // Wait a while

    setSpeed(0);

    delay(1000); // Wait a while
} 

void loop() 
{
    static float throttle;

    if (rx.timedOut(micros())) {
        Serial.println("*** TIMED OUT ***");
    }

    else if (rx.gotNewFrame()) {

        float rxvalues[CHANNELS];

        rx.getChannelValuesNormalized(rxvalues, CHANNELS);

        throttle = (rxvalues[0] + 1) / 2;

    }

    setSpeed(throttle);

    delay(10);
}
