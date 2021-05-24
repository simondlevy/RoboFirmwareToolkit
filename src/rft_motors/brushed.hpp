/*
   A class for brushed motors

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "rft_motors/real.hpp"
#include "rft_motors/rotary.hpp"

namespace rft {

    class BrushedMotor : public RealMotor, public RotaryMotor {

        public:

            BrushedMotor(uint8_t pin)
                : RealMotor(pin)
            {
            }

            virtual void begin(void) override 
            {
                analogWriteFrequency(_pin, 10000);
                analogWrite(_pin, 0);
            }

            virtual void write(float value) override
            {
                analogWrite(_pin, (uint8_t)(value * 255));
            }

    }; // class BrushedMotor

} // namespace rft
