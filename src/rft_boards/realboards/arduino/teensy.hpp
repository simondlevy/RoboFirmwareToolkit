/*
   Teensy 4.0 implementation of Board routines

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include <Wire.h>
#include "rft_boards/realboards/arduino.hpp"

namespace rft {

    class Teensy40 : public ArduinoBoard {

         public:

            Teensy40(void) 
                : ArduinoBoard(13)
            {
            }

    }; // class Teensy40

} // namespace rft
