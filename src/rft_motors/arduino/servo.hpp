/*
   A class for servo motors on Arduino

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "rft_motors/servo.hpp"

#include <Servo.h>

namespace rft {

    class ArduinoServoMotor : public ServoMotor {

        private:

            Servo _servo;

        public:

            ArduinoServoMotor(uint8_t pin)
                : ServoMotor(pin)
            {
            }

            virtual void begin(void) override
            {
                _servo.attach(_pin);
            }

            virtual void write(float value) override
            {
                _servo.write((uint8_t)((value + 1) * 90));
            }

    }; // class ArduinoServoMotor

} // namespace rft