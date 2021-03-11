/*
   Superclass for Arduino-based flight controllers

   Copyright (c) 2021 Simon D. Levy

   MIT Press
   */

#pragma once

#include "boards/realboard.hpp"

namespace rft {

    class ArduinoBoard : public RealBoard {

        private:

            uint8_t _led_pin = 0;
            bool    _led_inverted = false;

            static void powerPin(uint8_t id, uint8_t value)
            {
                pinMode(id, OUTPUT);
                digitalWrite(id, value);
            }

        protected:

            void begin(void)
            {
                pinMode(_led_pin, OUTPUT);
                digitalWrite(_led_pin, _led_inverted ? HIGH : LOW);

                Serial.begin(115200);
                RealBoard::begin();
            }

            void setLed(bool isOn) 
            { 
                digitalWrite(_led_pin, isOn ?  (_led_inverted?LOW:HIGH) : (_led_inverted?HIGH:LOW));
            }

            uint8_t serialNormalAvailable(void)
            {
                return Serial.available();
            }

            uint8_t serialNormalRead(void)
            {
                return Serial.read();
            }

            void serialNormalWrite(uint8_t c)
            {
                Serial.write(c);
            }

        public:

            static void powerPins(uint8_t pwr, uint8_t gnd)
            {
                powerPin(pwr, HIGH);
                powerPin(gnd, LOW);
            }

            ArduinoBoard(uint8_t ledPin, bool ledInverted=false)
            {
                _led_pin = ledPin;
                _led_inverted = ledInverted;

            }

    }; // class ArduinoBoard

    void Board::outbuf(char * buf)
    {
        Serial.print(buf);
    }

} // namespace rft
