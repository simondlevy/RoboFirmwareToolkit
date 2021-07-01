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

            virtual HardwareSerial * getSerial(uint8_t port)
            {
                return NULL;
            }

            uint8_t serialAvailable(uint8_t port)
            {
                if (port > 0) {
                    HardwareSerial * serial = getSerial(port);
                    return serial ? serial->available() : 0;
                }

                return Serial.available();
            }

            uint8_t serialRead(uint8_t port)
            {
                if (port > 0) {
                    HardwareSerial * serial = getSerial(port);
                    return serial ? serial->read() : 0;
                }

                return Serial.read();
            }

            void serialWrite(uint8_t byte, uint8_t port)
            {
                if (port > 0) {
                    HardwareSerial * serial = getSerial(port);
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
