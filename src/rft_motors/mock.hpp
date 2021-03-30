/*
   Mock motor for testing

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "RFT_motor.hpp"

namespace rft {

    class MockMotor : public Motor {

        public:

            MockMotor(void) 
                : Motor(NULL, 0)
            {
            }

            virtual void beginPin(uint8_t pin) override
            {
                (void)pin;
            }

            virtual void writePin(uint8_t pin, float value) override
            {
                (void)pin;
                (void)value;
            }

    }; // class MockMotor

} // namespace rft
