/*
   Actuator class for mixers, etc.

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

namespace rft {

    class Actuator {

        friend class RFT;
        friend class SerialTask;

        // XXX protected:
        public:

            virtual void begin(void) { }

            virtual void runDisarmed(void) { }

            virtual void cut(void) { }

        public:

            virtual void run(float * demands) = 0;

    }; // class Actuator

} // namespace rft
