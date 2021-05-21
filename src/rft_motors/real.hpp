/*
   Abstract class for motors

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "RFT_motor.hpp"

namespace rft {

    class RealMotor : public Motor {

        protected:

            uint8_t _pin = 0;

            RealMotor(uint8_t pin)
            {
                _pin = pin;
            }

        public:

            virtual void begin(void) { }

    }; // class RealMotor

} // namespace rft
