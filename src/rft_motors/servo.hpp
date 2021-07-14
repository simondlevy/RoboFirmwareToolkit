/*
   A class for servo motors

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "RFT_motor.hpp"

#include <Servo.h>

namespace rft {

    void Board::outbuf(char * s)
    {
        Serial.print(s);
    }

    class ServoMotor : public Motor {

        private:

            Servo _servo;

        public:

            ServoMotor(uint8_t pin)
                : Motor(pin)
            {
            }

            virtual void begin(void) override
            {
                _servo.attach(_pin);
            }

            virtual void write(float value) override
            {
                _servo.write((uint8_t)((value + 1) * 90));
            }

    }; // class ServoMotor

} // namespace rft
