/*
   Teensy 4.0 implementation of Board routines

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include <Wire.h>
#include "rft_boards/realboards/arduino_serial/arduino.hpp"

namespace rft {

    class Teensy40 : public ArduinoBoard {

         public:

            Teensy40(HardwareSerial * serial=NULL) 
                : ArduinoBoard(13, false, serial) // false = non-inverted LED
            {
            }

    }; // class Teensy40

} // namespace rft
