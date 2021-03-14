/*
   Timer task for serial comms

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include <RFT_timertask.hpp>
#include <RFT_board.hpp>
#include <RFT_debugger.hpp>
#include <RFT_actuator.hpp>

#include "parser.hpp"

namespace rft {

    class SerialTask {

        protected:

            static constexpr float FREQ = 66;

            Actuator * _actuator = NULL;
            OpenLoopController * _olc = NULL;
            State * _state = NULL;

            /*
            // TimerTask overrides -------------------------------------------------------

            virtual void doTask(void) override
            {
                while (_board->serialAvailableBytes() > 0) {

                    Parser::parse(_board->serialReadByte());
                }

                while (Parser::availableBytes() > 0) {
                    _board->serialWriteByte(Parser::readByte());
                }

                // Support motor testing from GCS
                if (!_state->armed) {
                    _actuator->runDisarmed();
                }
            }
            */

    };  // SerialTask

} // namespace rft
