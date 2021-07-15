/*
   Arduino sketch to test a servo

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#include "RFT_full.hpp"
#include "rft_motors/arduino/servo.hpp"

static uint8_t PIN = 9; // 3, 4, 5, 6, 8, 9

static float  val;
static int8_t dir;

static rft::ArduinoServoMotor motor = rft::ArduinoServoMotor(PIN);

void setup(void)
{
    // Start with motor off, increasing
    val = 0;
    dir = +1;

    motor.begin();

    // Necessary for some servos
    delay(4000);
}

void loop(void)
{
    motor.write(val);

    val += dir * .1;

    if (val >= +1) {
        dir = -1;
    }

    if (val <= -1) {
        dir = +1;
    }

    delay(25);
}
