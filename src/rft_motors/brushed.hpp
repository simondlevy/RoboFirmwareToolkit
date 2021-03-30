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

            virtual void beginPin(uint8_t pin) override
            {
                analogWriteFrequency(pin, 10000);  
                analogWrite(pin, 0);  
            }

            virtual void writePin(uint8_t pin, float value) override
            {
                analogWrite(pin, (uint8_t)(value * 255));
            }

    }; // class BrushedMotor

} // namespace rft
