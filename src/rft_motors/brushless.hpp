/*
   Arduino code for brushless motor running on standard ESC

   Copyright (c) 2021 Juan Gallostra Acin, Simon D. Levy, Pep Martí Saumell

   Simon D. Levy
*/

#pragma once

#include "rft_motors/real.hpp"
#include "rft_motors/rotary.hpp"

#ifdef ESP32
#include <analogWrite.h>
#endif

namespace rft {

    class BrushlessMotor : public RealMotor, public RotaryMotor {

        private:

#ifdef ESP32
            static const uint16_t OFFSET = 25;
#else
            static const uint16_t OFFSET = 0;
#endif
            static const uint16_t MINVAL = 125;
            static const uint16_t MAXVAL = 250;

            void writeValue(uint16_t value)
            {
                analogWrite(_pin, value+OFFSET);
            }

        public:

            BrushlessMotor(uint8_t pin)
                : RealMotor(pin)
            {
            }

            virtual void begin(void) override
            {
                pinMode(_pin, OUTPUT);
                writeValue(_pin, MINVAL);
            }

            virtual void write(float value) override
            { 
                writeValue(_pin, (uint16_t)(MINVAL+value*(MAXVAL-MINVAL))); 
            }

    }; // class BrushlessMotor

} // namespace rft
