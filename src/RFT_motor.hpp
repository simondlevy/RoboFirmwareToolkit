/*
   Abstract class for motors

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include <RFT_filters.hpp>

namespace rft {

    class Motor {

        private:

            uint8_t _pin = 0;

        public:

            void Motor(uint8_t pin)
            {
                _pin = pin;
            }

            virtual void begin(void) { }
            
            virtual void write(float value) = 0;

            virtual float constrainValue(float value) { return value; }

    }; // class Motor

} // namespace rft
