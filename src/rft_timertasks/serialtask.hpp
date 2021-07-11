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
#include <rft_boards/realboard.hpp>

namespace rft {

    class SerialTask : public TimerTask, public Parser {

        protected:

            static constexpr float FREQ = 66;

            bool _useTelemetryPort = false;

            SerialTask(bool secondaryPort=false)
                : TimerTask(FREQ)
            {
                _useTelemetryPort = secondaryPort;
            }

            virtual void doTask(Board * board,
                                OpenLoopController * olc,
                                Actuator * actuator,
                                State * state) override
            {
                (void)olc;
                (void)actuator;

                RealBoard * realboard = (RealBoard *)board;

                while (realboard->serialAvailable(_useTelemetryPort) > 0) {
                    Parser::parse(realboard->serialRead(_useTelemetryPort));
                }

                while (Parser::availableBytes() > 0) {
                    realboard->serialWrite(Parser::readByte(),
                                           _useTelemetryPort);
                }

                // Support motor testing from GCS
                if (!state->armed) {
                    actuator->runDisarmed();
                }
            }

    };  // SerialTask

} // namespace rft
