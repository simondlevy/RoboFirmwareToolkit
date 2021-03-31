/*
   Arduino code for brushless motor running on standard ESC

   Copyright (c) 2021 Juan Gallostra Acin, Simon D. Levy, Pep Martí Saumell

   Simon D. Levy
*/

#pragma once

#include "rft_motors/RFT_realmotor.hpp"

#ifdef ESP32
#include <analogWrite.h>
#endif

namespace rft {

    class StandardMotor : public RealMotor {

        private:

#ifdef ESP32
            static const uint16_t OFFSET = 25;
#else
            static const uint16_t OFFSET = 0;
#endif
            static const uint16_t MINVAL = 125;
            static const uint16_t MAXVAL = 250;

            void writeValue(uint8_t pin, uint16_t value)
            {
                analogWrite(pin, value+OFFSET);
            }

        public:

            StandardMotor(const uint8_t pins[], const uint8_t count) 
                : RealMotor(pins, count)
            {
            }

            virtual void beginPin(uint8_t pin) override
            {
                pinMode(pin, OUTPUT);
                writeValue(pin, MINVAL);
            }

            virtual void writePin(uint8_t pin, float value) override
            { 
                writeValue(pin, (uint16_t)(MINVAL+value*(MAXVAL-MINVAL))); 
            }

    }; // class StandardMotor

} // namespace rft
