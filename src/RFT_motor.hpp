/*
   Abstract parent class for running motors on Arduino

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#ifdef ESP32
#include <analogWrite.h>
#endif

namespace rft {

    class Motor {

        private:

            static const uint8_t MAX_COUNT = 20; // arbitrary

            uint8_t _pins[MAX_COUNT];

            uint8_t _count = 0;

        protected:

            Motor(const uint8_t count) 
            {
                _count = count;
            }

            Motor(const uint8_t * pins, const uint8_t count)
            {
                for (uint8_t k=0; k<count; ++k) {
                    _pins[k] = pins[k];
                }
                _count = count;
            }

            virtual void beginPin(uint8_t pin) = 0;
            virtual void writePin(uint8_t pin, float value) = 0;

        public:

            void begin(void)
            {
                for (uint8_t k=0; k<_count; ++k) {
                    beginPin(_pins[k]);
                }
            }

            void write(uint8_t index, float value) 
            {
                writePin(_pins[index], value);
            }

    }; // class Motor

} // namespace rft
