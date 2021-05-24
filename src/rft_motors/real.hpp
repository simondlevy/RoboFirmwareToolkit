/*
   Abstract class for real motors (using a specific pin)

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

namespace rft {

    class RealMotor {

        protected:

            uint8_t _pin = 0;

            RealMotor(uint8_t pin)
            {
                _pin = pin;
            }

        public:

            virtual void begin(void) { }

    }; // class RealMotor

} // namespace rft
