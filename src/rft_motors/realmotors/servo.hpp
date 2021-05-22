/*
   A class for servo motors

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "rft_motors/real.hpp"
#include "rft_filters.hpp"

#include <Servo.h>

namespace rft {

    class ServoMotor : public RealMotor {

        private:

            Servo _servo;

        public:

            ServoMotor(uint8_t pin)
                : RealMotor(pin)
            {
            }

            virtual void begin(void) override 
            {
                _servo.attach(_pin);
            }

            virtual void write(float value) override
            {
                // [-.5,+.5] => [0,180]
                _servo.write((uint8_t)(180*(value+0.5)));
            }

            virtual float constrainValue(float value) override
            {
                return Filter::constrainAbs(value, 0.45);
            }

    }; // class ServoMotor

} // namespace rft
