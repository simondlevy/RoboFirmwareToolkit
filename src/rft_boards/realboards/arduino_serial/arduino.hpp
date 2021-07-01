/*
   Superclass for Arduino-based flight controllers

   Copyright (c) 2021 Simon D. Levy

   MIT License
*/

#pragma once

#include "rft_boards/realboards/arduino_serial.hpp"

namespace rft {

    class ArduinoBoard : public ArduinoSerial {

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

                ArduinoSerial::begin();
            }

            void setLed(bool isOn) 
            { 
                digitalWrite(_led_pin, isOn ?  (_led_inverted?LOW:HIGH) : (_led_inverted?HIGH:LOW));
            }

        public:

            static void powerPins(uint8_t pwr, uint8_t gnd)
            {
                powerPin(pwr, HIGH);
                powerPin(gnd, LOW);
            }

            ArduinoBoard(uint8_t ledPin, bool ledInverted=false, HardwareSerial * serial=NULL)
                : ArduinoSerial(serial)
            {
                _led_pin = ledPin;
                _led_inverted = ledInverted;

            }

    }; // class ArduinoBoard

} // namespace rft
