/*
   A class for servo motors

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "rft_motors/mock.hpp"
#include "rft_motors/servo.hpp"

namespace rft {

    class MockServo : public MockMotor, public ServoMotor {


    }; // class MockServo

} // namespace rft
