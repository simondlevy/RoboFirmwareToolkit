/*
   Class for Arduino-style serial comms

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "rft_boards/realboard.hpp"

namespace rft {

    class ArduinoSerial : public RealBoard {

        protected:

            uint8_t serialAvailable(uint8_t uart=0)
            {
                return Serial.available();
            }

            uint8_t serialRead(uint8_t uart=0)
            {
                return Serial.read();
            }

            void serialWrite(uint8_t c, uint8_t uart=0)
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

    }; // class ArduinoSerial

    void Board::outbuf(char * buf)
    {
        Serial.print(buf);
    }

} // namespace rft
