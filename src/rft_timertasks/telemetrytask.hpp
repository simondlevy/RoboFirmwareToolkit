/*
   Timer task for telemetry

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include <RFT_timertask.hpp>
#include <RFT_board.hpp>
#include <RFT_debugger.hpp>
#include <RFT_actuator.hpp>
#include <RFT_parser.hpp>
#include <RFT_state.hpp>

namespace rft {

    class TelemetryTask : public TimerTask, public Parser {

        protected:

            static constexpr float FREQ = 66;

            TelemetryTask(void)
                : TimerTask(FREQ)
            {
            }

            void begin(rft::Board * board, rft::State * state) 
            {
                TimerTask::begin(board);
            }

            virtual void doTask(OpenLoopController * olc, State * state) override
            {
                (void)state;
                (void)olc;

                while (_board->telemetryAvailable() > 0) {

                    Parser::parse(_board->telemetryRead());
                }

                while (Parser::availableBytes() > 0) {
                    _board->telemetryWrite(Parser::readByte());
                }
            }

    };  // TelemetryTask

} // namespace rft
