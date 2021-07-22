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

            STM32L4(int8_t ledPin = -13, // inverted
                    HardwareSerial * telemetryPort = NULL) 
                : ArduinoBoard(ledPin, telemetryPort) 
            {
            }

    }; // class STM32L4

} // namespace rft
