/*
   Arduino code for brushless motor running on standard ESC

   Copyright (c) 2021 Juan Gallostra Acin, Simon D. Levy, Pep Martí Saumell

   Simon D. Levy
*/

#pragma once

#include "rft_motors/rotary.hpp"

#include <Servo.h>

namespace rft {

    class BrushlessMotor : public RotaryMotor {

        private:

            Servo _esc;

        public:

            BrushlessMotor(uint8_t pin)
                : RotaryMotor(pin)
            {
            }

            virtual void begin(void) override
            {
                _esc.attach(_pin);
                _esc.write(0);
            }

            virtual void write(float value) override
            { 
                _esc.write((uint8_t)(180 * value));
            }

    }; // class BrushlessMotor

} // namespace rft
