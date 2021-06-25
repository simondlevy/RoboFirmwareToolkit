/*
   Arduino sketch to test brushless motor with standard ESC

   DID YOU REMEMOVE THE PROPELLERS FIRST?

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#include "RoboFirmwareToolkit.hpp"
#include "rft_motors/rotary/brushless.hpp"

static uint8_t MOTOR_PIN = 5;

static float  val;
static int8_t dir;

rft::BrushlessMotor motor = rft::BrushlessMotor(MOTOR_PIN);

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
