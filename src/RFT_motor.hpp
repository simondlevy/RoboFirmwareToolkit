/*
   Abstract class for motors

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once


namespace rft {

    class Motor {

        public:

            virtual void begin(void) { }
            
            virtual void write(float value) = 0;

    }; // class Motor

} // namespace rft
