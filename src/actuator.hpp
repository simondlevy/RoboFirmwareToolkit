/*
   Actuator class for mixers, etc.

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

namespace rft {

    class Actuator {

        friend class Toolkit;
        friend class SerialTask;

        protected:

            virtual void begin(void) { }

            virtual void runDisarmed(void) { }

            virtual void cut(void) { }

            virtual void setMotorDisarmed(uint8_t index, float value) { }

        public:

            virtual void run(demands_t demands) = 0;

    }; // class Actuator

} // namespace rft
