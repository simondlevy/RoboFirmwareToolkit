/*
   A class for servo motors

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "rft_motors/real.hpp"
#include "rft_motors/servo.hpp"

#include <Servo.h>

namespace rft {

    class RealServo : public RealMotor, public ServoMotor {

        private:

            Servo _servo;

        public:

            RealServo(uint8_t pin)
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

    }; // class RealServo

} // namespace rft
