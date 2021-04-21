/*
   A closed-loop controller that passes demands through unmodified

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "RFT_closedloop.hpp"

namespace rft {

    class PassthruController : public ClosedLoopController {

            virtual void modifyDemands(State * state, float * demands) override
            {
                (void)state;
                (void)demands;
            }

    };  // class PassthruController

} // namespace rft
