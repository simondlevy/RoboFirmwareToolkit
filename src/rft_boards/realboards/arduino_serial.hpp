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

    }; // class ArduinoSerial

    void Board::outbuf(char * buf)
    {
        Serial.print(buf);
    }

} // namespace rft
