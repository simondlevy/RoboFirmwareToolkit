/*
   Abstract class for Open-Loop Controllers 

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include <stdint.h>
#include <math.h>

#include "demands.hpp"

namespace rft {

    class OpenLoopController {

        friend class Toolkit;
        friend class SerialTask;
        friend class PidTask;

        protected: 

            virtual void getDemands(demands_t & demands) = 0;

            virtual void begin(void) 
            { 
            }

            virtual float getRawval(uint8_t chan)
            {
                return 0;
            }

            virtual bool lostSignal(void) 
            { 
                return false; 
            }

            virtual bool ready(void)
            {
                return true;
            }

            virtual bool inactive(void) 
            {
                return false;
            }

            virtual bool inArmedState(void)
            {
                return true;
            }

            virtual uint8_t getModeIndex(void)
            {
                return 0;
            }

    }; // class OpenLoopController

} // namespace rft
