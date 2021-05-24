/*
   A class for rotary motors

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "RFT_motor.hpp"

namespace rft {

    class RotaryMotor : public Motor {

        public:

            virtual float constrainValue(float value) override
            {
                return Filter::constrainMinMax(value, 0, 1);
            }

    }; // class RotaryMotor

} // namespace rft
