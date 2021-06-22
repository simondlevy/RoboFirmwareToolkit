/*
   Arduino code for brushless motor running on standard ESC

   Copyright (c) 2021 Juan Gallostra Acin, Simon D. Levy, Pep Martí Saumell

   Simon D. Levy
*/

#pragma once

#include "rft_motors/rotary.hpp"

#ifdef ESP32
#include <analogWrite.h>
#endif

namespace rft {

    class BrushlessMotor : public RotaryMotor {

        private:

            static const uint16_t MINVAL = 125;
            static const uint16_t MAXVAL = 250;

            uint16_t _offset = 0;

            void writeValue(uint16_t value)
            {
                analogWrite(_pin, value + _offset);
            }

        protected:

            BrushlessMotor(uint8_t pin, uint16_t offset)
                : RealMotor(pin)
            {
                _offset = offset;
            }

        public:

            BrushlessMotor(uint8_t pin)
                : BrushlessMotor(pin, 0)
            {
            }

            virtual void begin(void) override
            {
                pinMode(_pin, OUTPUT);
                writeValue(MINVAL);
            }

            virtual void write(float value) override
            { 
                writeValue((uint16_t)(MINVAL+value*(MAXVAL-MINVAL))); 
            }

    }; // class BrushlessMotor

    class BrushlessMotorEsp32 : public BrushlessMotor {

        public:

            BrushlessMotorEsp32(uint8_t pin)
                : BrushlessMotor(pin, 25)
            {
            }

    };  // class BrushlessMotorEsp32

} // namespace rft
