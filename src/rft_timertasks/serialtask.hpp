/*
   Telemetry task for serial comms

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include <RFT_timertask.hpp>
#include <RFT_debugger.hpp>
#include <RFT_actuator.hpp>
#include <RFT_parser.hpp>
#include <RFT_boards/realboard.hpp>

namespace rft {

    class SerialTask : public TimerTask, public Parser {

        protected:

            static constexpr float FREQ = 66;

            RealBoard * _realboard = NULL;

            State * _state = NULL;

            Actuator * _actuator = NULL;

            OpenLoopController * _olc = NULL;

            uint8_t _port = 0;

            SerialTask(uint8_t port=0)
                : TimerTask(FREQ)
            {
                _port = port;
            }

            void begin(Board * board, State * state, OpenLoopController * olc, Actuator * actuator) 
            {
                TimerTask::begin(board);

                _realboard = (RealBoard *)board;

                _state = state;
                _olc = olc;
                _actuator = actuator;
            }

            virtual void doTask(void) override
            {
                while (_realboard->serialAvailable(_port) > 0) {

                    Parser::parse(_realboard->serialRead(_port));
                }

                while (Parser::availableBytes() > 0) {
                    _realboard->serialWrite(Parser::readByte(), _port);
                }
                 // Support motor testing from GCS
                if (!_state->armed) {
                    _actuator->runDisarmed();
                }
            }

    };  // SerialTask

} // namespace rft
