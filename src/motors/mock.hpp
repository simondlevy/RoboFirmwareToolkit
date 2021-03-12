/*
   Mock motor for testing

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "motor.hpp"

namespace rft {

    class MockMotor : public Motor {

        public:

            MockMotor(void) 
                : Motor(NULL, 0)
            {
            }

            virtual void begin(void) override
            {
            }

            virtual void write(uint8_t index, float value) override
            {
                (void)index;
                (void)value;
            }

    }; // class MockMotor

} // namespace rft
