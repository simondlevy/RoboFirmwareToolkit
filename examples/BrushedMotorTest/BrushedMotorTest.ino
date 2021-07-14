/*
   Arduino sketch to test brushed motor

   DID YOU REMEMOVE THE ROTORS FIRST?

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#include "RFT_full.hpp"
#include "rft_motors/rotary/brushed.hpp"

static uint8_t PIN = 3;

static float  val;
static int8_t dir;

static rft::BrushedMotor motor = rft::BrushedMotor(PIN);

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
