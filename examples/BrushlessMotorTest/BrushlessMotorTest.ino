/*
   Arduino sketch to test brushless motor with standard ESC

   DID YOU REMEMOVE THE PROPELLERS FIRST?

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#include "RFT_main.hpp"
#include "rft_motors/standard.hpp"

static uint8_t MOTOR_PIN[1] = {5};

static float  val;
static int8_t dir;

rft::StandardMotor motors = rft::StandardMotor(MOTOR_PIN, 1);

void setup(void)
{
    // Initialize the motor
    motors.begin();

    // Start with motor off, increasing
    val = 0;
    dir = +1;

    delay(1000);
}

void loop(void)
{
    motors.write(0, val);

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
