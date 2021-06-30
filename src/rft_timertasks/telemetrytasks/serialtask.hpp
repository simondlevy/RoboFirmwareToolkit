/*
   Telemetry task for serial comms

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include <rft_timertasks/telemetrytask.hpp>

namespace rft {

    class SerialTask : public TelemetryTask {

        protected:


            Actuator * _actuator = NULL;

            OpenLoopController * _olc = NULL;

            SerialTask(void)
                : TelemetryTask(0) // UART0
            {
            }

            void begin(rft::Board * board, rft::State * state, rft::OpenLoopController * olc, rft::Actuator * actuator) 
            {
                TelemetryTask::begin(board, state);

                _olc = olc;

                _actuator = actuator;
            }

            virtual void doTask(void) override
            {
                TelemetryTask::doTask();

                // Support motor testing from GCS
                if (!_state->armed) {
                    _actuator->runDisarmed();
                }
            }

    };  // SerialTask

} // namespace rft
