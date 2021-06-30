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

namespace rft {

    class TelemetryTask : public TimerTask, public Parser {

        protected:

            static constexpr float FREQ = 66;

            State * _state = NULL;

            uint8_t _uart = 0;

            TelemetryTask(uint8_t uart=0)
                : TimerTask(FREQ)
            {
                _uart = uart;
            }

            void begin(rft::Board * board, rft::State * state) 
            {
                TimerTask::begin(board);
                _state = state;
            }

            virtual void doTask(void) override
            {
                while (_board->serialAvailable(_uart) > 0) {

                    Parser::parse(_board->serialRead(_uart));
                }

                while (Parser::availableBytes() > 0) {
                    _board->serialWrite(Parser::readByte());
                }
            }

    };  // TelemetryTask

} // namespace rft
