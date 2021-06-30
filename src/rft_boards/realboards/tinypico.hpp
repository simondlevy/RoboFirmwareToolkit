/*
   TinyPICO implementation of Board routines, with IMU mounted on bottom of board

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include <Wire.h>
#include <TinyPICO.h>

#include "rft_boards/realboards/arduino.hpp"

namespace rft {

    class TinyPico : public RealBoard {

        private:

            TinyPICO tp;

        protected:

            void setLed(bool isOn) 
            { 
                tp.DotStar_SetPixelColor(0, isOn?255:0, 0);
            }

            uint8_t serialAvailable(void)
            {
                return Serial.available();
            }

            uint8_t serialRead(void)
            {
                return Serial.read();
            }

            void serialWrite(uint8_t c)
            {
                Serial.write(c);
            }

            void begin(void)
            {
                // Start serial communcation for GCS/debugging
                Serial.begin(115200);

                // This will blink the LED
                RealBoard::begin();
            }

    }; // class TinyPico

} // namespace rft
