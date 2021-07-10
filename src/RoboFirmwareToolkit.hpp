/*
   RFT class for RoboFirmwareToolkit

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "RFT_board.hpp"
#include "RFT_openloop.hpp"
#include "RFT_closedloop.hpp"
#include "RFT_sensor.hpp"
#include "RFT_actuator.hpp"
#include "RFT_parser.hpp"
#include "rft_timertasks/closedlooptask.hpp"

namespace rft {

    class RFT {

        private:

            // Safety
            bool _safeToArm = false;

            // Sensors 
            Sensor * _sensors[256] = {};
            uint8_t _sensor_count = 0;

            // Timer task for PID controllers
            ClosedLoopTask _closedLoopTask;

            void startSensors(void) 
            {
                for (uint8_t k=0; k<_sensor_count; ++k) {
                    _sensors[k]->begin();
                }
            }

            void checkSensors(Board * board, State * state)
            {
                // Some sensors may need to know the current time
                float time = board->getTime();

                for (uint8_t k=0; k<_sensor_count; ++k) {
                    _sensors[k]->modifyState(state, time);
                }
            }

            void checkOpenLoopController(Board * board, OpenLoopController * olc, State * state)
            {
                // Sync failsafe to open-loop-controller
                if (olc->lostSignal() && state->armed) {
                    _actuator->cut();
                    state->armed = false;
                    state->failsafe = true;
                    board->showArmedStatus(false);
                    return;
                }

                // Check whether controller data is available
                if (!olc->ready()) return;

                // Disarm
                if (state->armed && !olc->inArmedState()) {
                    state->armed = false;
                } 

                // Avoid arming when controller is in armed state
                if (!_safeToArm) {
                    _safeToArm = !olc->inArmedState();
                }

                // Arm after lots of safety checks
                if (_safeToArm
                    && !state->armed
                    && !state->failsafe 
                    && state->safeToArm()
                    && olc->inactive()
                    && olc->inArmedState()
                    ) {
                    state->armed = true;
                }

                // Cut motors on inactivity
                if (state->armed && olc->inactive()) {
                    _actuator->cut();
                }

                // Set LED based on arming status
                board->showArmedStatus(state->armed);

            } // checkOpenLoopController

        protected:

            rft::Actuator * _actuator = NULL;

            RFT(Actuator * actuator)
            {
                // Store the essentials
                _actuator = actuator;

                // Support adding new sensors
                _sensor_count = 0;
            }

            void begin(Board * board, OpenLoopController * olc, State * state, bool armed=false)
            {  
                // Zero-out the state
                memset(state, 0, sizeof(*state));

                // Start the board
                board->begin();

                // Initialize the sensors
                startSensors();

                // Initialize the open-loop controller
                olc->begin();

                // Start the actuator
                _actuator->begin();

                // Initialize failsafe
                state->failsafe = false;

                // Initialize timer task for PID controllers
                _closedLoopTask.begin(board, _actuator);

                // Support safety override by simulator
                state->armed = armed;

            } // begin

            void update(Board * board, OpenLoopController * olc, State * state)
            {
                // Grab control signal if available
                checkOpenLoopController(board, olc, state);

                // Update PID controllers task
                _closedLoopTask.update(olc, state);

                // Check sensors
                checkSensors(board, state);
            }

        public:

            void addSensor(Sensor * sensor) 
            {
                _sensors[_sensor_count++] = sensor;
            }

            void addClosedLoopController(rft::ClosedLoopController * controller, uint8_t modeIndex=0) 
            {
                _closedLoopTask.addController(controller, modeIndex);
            }

    }; // class RFT

} // namespace
