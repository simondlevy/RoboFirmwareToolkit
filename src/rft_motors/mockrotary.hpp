/*
   Mock implementation of rotary motors for testing and simulation

   Copyright(C) 2021 Simon D.Levy

   MIT License
   */

#pragma once

#include "rft_motors/mock.hpp"
#include "rft_motors/rotary.hpp"

namespace rft {

    class MockRotaryMotor : public MockMotor, public RotaryMotor {

        protected:

            virtual void write(float value) override
            {
                MockMotor::write(value);
            }


    }; // class MockRotaryMotor

} // namespace rft
