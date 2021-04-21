/*
   Mock actuator class for prototyping

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "rft_actuator.hpp"

namespace rft {

    class MockActuator : public Actuator {

        public:

            virtual void run(float * demands) override
            {
            }

    }; // class MockActuator

} // namespace rft
