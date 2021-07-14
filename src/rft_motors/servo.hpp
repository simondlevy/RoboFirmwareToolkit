/*
   A class for servo motors

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "RFT_motor.hpp"

#include <Servo.h>

namespace rft {

    class ServoMotor : public Motor {

        private:

            Servo _servo;

        public:

            ServoMotor(uint8_t pin)
                : Motor(pin)
            {
            }

            virtual void begin(void) override
            {
                _servo.attach(_pin);
            }

            virtual float constrainValue(float value) override
            {
                return Filter::constrainAbs(value, 0.45);
            }

            virtual void write(float value) override
            {
                _servo.write((uint8_t)(90 + value * 45));
            }

    }; // class ServoMotor

} // namespace rft
