/*
   Abstract class for sensors

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "RFT_state.hpp"

namespace rft {

    class Sensor {

        friend class RFT;

        // XXX protected:
        public:

            virtual void begin(void) { }

            virtual void modifyState(State * state, float time) = 0;

            virtual bool ready(float time) = 0;

    };  // class Sensor

} // namespace rft
