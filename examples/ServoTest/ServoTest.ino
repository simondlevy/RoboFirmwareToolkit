/*
   Arduino sketch to test a servo

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#include <Servo.h>

static uint8_t PIN = 5;

static uint8_t  val;
static int8_t dir;

static Servo servo;

void setup(void)
{
    servo.attach(PIN);

    // Start with motor off, increasing
    val = 90;
    dir = +1;

    // Necessary for some servos
    delay(4000);
}

void loop(void)
{
    servo.write(val);

    val += dir;

    if (val == 180) {
        dir = -1;
    }

    if (val == 0) {
        dir = +1;
    }

    delay(25);
}
