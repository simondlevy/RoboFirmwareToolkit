/*
   Mock implementation of servo motors for testing and simulation

   Copyright(C) 2021 Simon D.Levy

   MIT License
   */

#pragma once

#include <rft_motors/mock.hpp>
#include <rft_motors/servo.hpp>

namespace rft {

    class MockServoMotor : public MockMotor, public ServoMotor {

        protected:

            virtual void write(float value) override
            {
                MockMotor::write(value);
            }

    }; // class MockServoMotor

} // namespace rft
