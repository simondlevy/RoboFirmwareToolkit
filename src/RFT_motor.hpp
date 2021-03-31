/*
   Abstract parent class for running motors on Arduino

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

namespace rft {

    class Motor {

        protected:

            uint8_t _count = 0;

            Motor(const uint8_t count)
            {
                _count = count;
            }

        public:

            virtual void begin(void)
            {
            }

            virtual void write(uint8_t index, float value) = 0;

    }; // class Motor

} // namespace rft
