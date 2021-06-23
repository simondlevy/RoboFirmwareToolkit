/*
   A class for servo motors

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "RFT_motor.hpp"

namespace rft {

    class ServoMotor : public Motor {

        public:

            ServoMotor(uint8_t pin)
                : Motor(pin)
            {
            }

            virtual float constrainValue(float value) override
            {
                return Filter::constrainAbs(value, 0.45);
            }

    }; // class ServoMotor

} // namespace rft
