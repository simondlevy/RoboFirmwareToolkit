/*
   A class for mock rotary motors (for testing)

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "rft_motors/rotary.hpp"
#include "rft_motors/mock.hpp"

namespace rft {

    class MockRotaryMotor : public MockMotor, public RotaryMotor {


    }; // class MockRotaryMotor

} // namespace rft
