/*
   Tlera STM32L4 implementation of Board routines

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include <Wire.h>
#include "rft_boards/realboards/arduino_serial/arduino.hpp"

namespace rft {

    class STM32L4 : public ArduinoBoard {

         public:

            STM32L4(HardwareSerial * telemetryPort = NULL) 
                : ArduinoBoard(13, true, telemetryPort) // red LED, active low
            {
            }

    }; // class STM32L4

} // namespace rft
