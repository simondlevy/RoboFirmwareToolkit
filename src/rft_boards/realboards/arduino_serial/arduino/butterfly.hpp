/*
   Tlera Butterfly implementation of Board routines

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include <Wire.h>
#include "rft_boards/realboards/arduino_serial/arduino.hpp"

namespace rft {

    class Butterfly : public ArduinoBoard {

         public:

            Butterfly(HardwareSerial * telemetryPort = NONE) 
                : ArduinoBoard(13, true, telemetryPort) // red LED, active low
            {
            }

    }; // class Butterfly

} // namespace rft
