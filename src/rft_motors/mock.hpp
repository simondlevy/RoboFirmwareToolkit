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

            virtual void write(float value) override
            {
                (void)value;
            }

            virtual float constrainValue(float value) override
            {
                (void)value;
                return 0;
            }

    }; // class MockMotor

} // namespace rft
