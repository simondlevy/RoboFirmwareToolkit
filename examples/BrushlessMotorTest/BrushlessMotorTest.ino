/*
   Arduino sketch to test brushless motor with standard ESC

   DID YOU REMEMOVE THE ROTORS FIRST?

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#include "RFT_full.hpp"
#include "rft_motors/arduino/brushless.hpp"

static uint8_t PIN = 5;

static float  val;
static int8_t dir;

static rft::ArduinoBrushlessMotor motor = rft::ArduinoBrushlessMotor(PIN);

void setup(void)
{
    // Initialize the motor
    motor.begin();

    // Start with motor off, increasing
    val = 0;
    dir = +1;

    delay(1000);
}

void loop(void)
{
    motor.write(val);

    val += dir * .001;

    // stop halfway
    if (val >= 0.5) {
        dir = -1;
    }

    if (val <= 0) {
        dir = +1;
    }

    delay(10);
}
