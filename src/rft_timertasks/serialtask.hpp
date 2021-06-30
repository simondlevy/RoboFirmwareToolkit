/*
   Telemetry task for serial comms

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include <RFT_timertask.hpp>
#include <RFT_board.hpp>
#include <RFT_debugger.hpp>
#include <RFT_actuator.hpp>
#include <RFT_parser.hpp>

namespace rft {

    class SerialTask : public TimerTask, public Parser {

        protected:

            static constexpr float FREQ = 66;

            State * _state = NULL;

            Actuator * _actuator = NULL;

            OpenLoopController * _olc = NULL;

            SerialTask(void)
                : TimerTask(FREQ)
            {
            }

            void begin(rft::Board * board, rft::State * state, rft::OpenLoopController * olc, rft::Actuator * actuator) 
            {
                TimerTask::begin(board);

                _state = state;
                _olc = olc;
                _actuator = actuator;
            }

            virtual void doTask(void) override
            {
                while (_board->serialAvailable() > 0) {

                    Parser::parse(_board->serialRead());
                }

                while (Parser::availableBytes() > 0) {
                    _board->serialWrite(Parser::readByte());
                }
                 // Support motor testing from GCS
                if (!_state->armed) {
                    _actuator->runDisarmed();
                }
            }

    };  // SerialTask

} // namespace rft
