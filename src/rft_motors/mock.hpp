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
                : Motor(0)
            {
            }

            virtual void write(uint8_t index, float value) override
            {
                (void)index;
                (void)value;
            }

    }; // class MockMotor

} // namespace rft
