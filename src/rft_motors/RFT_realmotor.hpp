/*
   Abstract parent class for running motors on Arduino

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "RFT_motor.hpp"

namespace rft {

    class RealMotor : public Motor {

        private:

            static const uint8_t MAX_COUNT = 20; // arbitrary

            uint8_t _pins[MAX_COUNT];

        protected:

            RealMotor(const uint8_t * pins, const uint8_t count)
                : Motor(count)
            {
                for (uint8_t k=0; k<count; ++k) {
                    _pins[k] = pins[k];
                }
            }

            virtual void beginPin(uint8_t pin) = 0;

            virtual void writePin(uint8_t pin, float value) = 0;

        public:

            virtual void begin(void) override
            {
                for (uint8_t k=0; k<_count; ++k) {
                    beginPin(_pins[k]);
                }
            }

            virtual void write(uint8_t index, float value) override
            {
                writePin(_pins[index], value);
            }

    }; // class Motor

} // namespace rft
