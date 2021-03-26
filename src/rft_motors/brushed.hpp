/*
   Arduino code for brushed motors

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "RFT_motor.hpp"

namespace rft {

    class BrushedMotor : public Motor {

        public:

            BrushedMotor(const uint8_t * pins, const uint8_t count) 
                : Motor(pins, count) 
            {
            }

            virtual void init(uint8_t index) override
            {
                analogWriteFrequency(_pins[index], 10000);  
                analogWrite(_pins[index], 0);  
            }

            virtual void write(uint8_t index, float value) override
            {
                analogWrite(_pins[index], (uint8_t)(value * 255));
            }

    }; // class BrushedMotor

} // namespace rft
