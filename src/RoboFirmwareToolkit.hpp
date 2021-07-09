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

            // Vehicle state (created by subclass)
            State * _state = NULL;

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

            void checkSensors(void)
            {
                // Some sensors may need to know the current time
                float time = _board->getTime();

                for (uint8_t k=0; k<_sensor_count; ++k) {
                    _sensors[k]->modifyState(_state, time);
                }
            }

            void checkOpenLoopController(void)
            {
                // Sync failsafe to open-loop-controller
                if (_olc->lostSignal() && _state->armed) {
                    _actuator->cut();
                    _state->armed = false;
                    _state->failsafe = true;
                    _board->showArmedStatus(false);
                    return;
                }

                // Check whether controller data is available
                if (!_olc->ready()) return;

                // Disarm
                if (_state->armed && !_olc->inArmedState()) {
                    _state->armed = false;
                } 

                // Avoid arming when controller is in armed state
                if (!_safeToArm) {
                    _safeToArm = !_olc->inArmedState();
                }

                // Arm after lots of safety checks
                if (_safeToArm
                    && !_state->armed
                    && !_state->failsafe 
                    && _state->safeToArm()
                    && _olc->inactive()
                    && _olc->inArmedState()
                    ) {
                    _state->armed = true;
                }

                // Cut motors on inactivity
                if (_state->armed && _olc->inactive()) {
                    _actuator->cut();
                }

                // Set LED based on arming status
                _board->showArmedStatus(_state->armed);

            } // checkOpenLoopController

        protected:

            Board * _board = NULL;

            OpenLoopController * _olc = NULL;

            rft::Actuator * _actuator = NULL;

            RFT(State * state, Board * board, OpenLoopController * olc, Actuator * actuator)
            {
                // Store the essentials
                _state = state;
                _board    = board;
                _olc = olc;
                _actuator = actuator;

                // Zero-out the state
                memset(state, 0, sizeof(*state));

                // Support adding new sensors
                _sensor_count = 0;
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

            void begin(bool armed=false)
            {  
                // Start the board
                _board->begin();

                // Initialize the sensors
                startSensors();

                // Initialize the open-loop controller
                _olc->begin();

                // Start the actuator
                _actuator->begin();

                // Initialize failsafe
                _state->failsafe = false;

                // Initialize timer task for PID controllers
                _closedLoopTask.begin(_board, _olc, _actuator, _state);

                // Support safety override by simulator
                _state->armed = armed;

            } // begin

            void update(void)
            {
                // Grab control signal if available
                checkOpenLoopController();

                // Update PID controllers task
                _closedLoopTask.update();

                // Check sensors
                checkSensors();
            }

    }; // class RFT

} // namespace
