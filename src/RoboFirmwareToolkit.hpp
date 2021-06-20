/*
   RFT class for RoboFirmwareToolkit

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "RFT_debugger.hpp"
#include "RFT_board.hpp"
#include "RFT_openloop.hpp"
#include "RFT_closedloop.hpp"
#include "RFT_sensor.hpp"
#include "RFT_actuator.hpp"
#include "RFT_parser.hpp"
#include "rft_timertasks/serialtask.hpp"
#include "rft_timertasks/closedlooptask.hpp"

namespace rft {

    class RFT {

        private:

            // Safety
            bool _safeToArm = false;

            // Sensors 
            Sensor * _sensors[256] = {};
            uint8_t _sensor_count = 0;

            void startSensors(void) 
            {
                for (uint8_t k=0; k<_sensor_count; ++k) {
                    _sensors[k]->begin();
                }
            }

            // Timer task for PID controllers
            ClosedLoopTask _closedLoopTask;

            Debugger _debugger;

        protected:

            Board * _board = NULL;

            OpenLoopController * _olc = NULL;

            rft::Actuator * _actuator = NULL;

            State * _state = NULL;

            RFT(State * state, Board * board, OpenLoopController * olc, Actuator * actuator)
            {
                // Store the essentials
                _state = state;
                _board    = board;
                _olc = olc;
                _actuator = actuator;

                // Support adding new sensors
                _sensor_count = 0;
            }

            virtual bool safeStateForArming(void) = 0;

            void checkSensors(void)
            {
                // Some sensors may need to know the current time
                float time = _board->getTime();

                for (uint8_t k=0; k<_sensor_count; ++k) {
                    rft::Sensor * sensor = _sensors[k];
                    if (sensor->ready(time)) {
                        sensor->modifyState(_state, time);
                    }
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

                // Arm (after lots of safety checks!)
                if (_safeToArm
                    && !_state->armed
                    && _olc->inactive()
                    && _olc->inArmedState()
                    && !_state->failsafe 
                    && safeStateForArming()
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

            void update(void)
            {
                // Grab control signal if available
                checkOpenLoopController();

                // Update PID controllers task
                _closedLoopTask.update();

                // Check sensors
                checkSensors();
            }

            void begin(bool armed=false)
            {  
                // Start the board
                _board->begin();

                // Ad-hoc debugging support
                _debugger.begin(_board);

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
