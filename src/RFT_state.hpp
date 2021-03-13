/*
   State datatype

   Copyright (c) 2021 Simon D. Levy

   MIT License
   */

#pragma once

namespace rft {

    class State {

        public:

            bool armed;
            bool failsafe;

            virtual bool safeToArm(void) = 0;

    }; // class State

} // namespace rft
