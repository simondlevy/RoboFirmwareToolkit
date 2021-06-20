/*
   Abstract class for Open-Loop Controllers 

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include <stdint.h>
#include <math.h>

namespace rft {

    class OpenLoopController {

        friend class RFT;
        friend class SerialTask;
        friend class ClosedLoopTask;

        // XXX protected:
        public:

            static const uint8_t MAX_DEMANDS = 10; // arbitrary

            virtual void getDemands(float * demands) = 0;

            virtual void begin(void) 
            { 
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

} // namespace
