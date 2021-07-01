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

            virtual HardwareSerial * getSecondarySerial(void)
            {
                return NULL;
            }

            uint8_t serialAvailable(bool secondaryPort)
            {
                if (secondaryPort) {
                    HardwareSerial * serial = getSecondarySerial();
                    return serial ? serial->available() : 0;
                }

                return Serial.available();
            }

            uint8_t serialRead(bool secondaryPort)
            {
                if (secondaryPort) {
                    HardwareSerial * serial = getSecondarySerial();
                    return serial ? serial->read() : 0;
                }

                return Serial.read();
            }

            void serialWrite(uint8_t byte, bool secondaryPort)
            {
                if (secondaryPort) {
                    HardwareSerial * serial = getSecondarySerial();
                    if (serial) {
                        serial->write(byte);
                    }
                }

                else {
                    Serial.write(byte);
                }
            }

            void begin(void)
            {
                // Start serial communcation for GCS/debugging
                Serial.begin(SERIAL_BAUD);

                // This will blink the LED
                RealBoard::begin();
            }

    }; // class ArduinoSerial

    void Board::outbuf(char * buf)
    {
        Serial.print(buf);
    }

} // namespace rft
