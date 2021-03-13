/*
   RFT class for RoboFirmwareToolkit

   Copyright (c) 2021 Simon D. Levy

   MIT License
*/

#pragma once

#include "RFT_debugger.hpp"
//#include "RFT_mspparser.hpp"
#include "RFT_board.hpp"
#include "RFT_openloop.hpp"
//#include "rft_states/mavstate.hpp"
#include "RFT_closedloop.hpp"
#include "RFT_sensor.hpp"
#include "RFT_actuator.hpp"
#include "rft_timertasks/closedlooptask.hpp"
//#include "rft_timertasks/serialtask.hpp"

namespace rft {

    class RFT {

        protected:

            Board * _board = NULL;

            OpenLoopController * _olc = NULL;

            rft::Actuator * _actuator = NULL;

            Debugger _debugger;

            // Sensors 
            Sensor * _sensors[256] = {NULL};
            uint8_t _sensor_count = 0;

            RFT(Board * board, OpenLoopController * olc, Actuator * actuator)
            {
                // Store the essentials
                _board    = board;
                _olc = olc;
                _actuator = actuator;

                // Support adding new sensors
                _sensor_count = 0;
            }

        /*
            // Supports periodic ad-hoc debugging
            // Safety
            bool _safeToArm = false;

            // Timer task for PID controllers
            ClosedLoopTask _closedLoopTask;

            // Passed to RFT::begin() for a particular build
            Actuator * _actuator = NULL;

            // Serial timer task for GCS
            SerialTask _serialTask;

            // Vehicle state
            MavState _state;

            void checkSensors(void)
            {
                for (uint8_t k=0; k<_sensor_count; ++k) {
                    Sensor * sensor = _sensors[k];
                    float time = _board->getTime();
                    if (sensor->ready(time)) {
                        sensor->modifyState(&_state, time);
                    }
                }
            }

            void checkOpenLoopController(void)
            {
                // Sync failsafe to open-loop-controller
                if (_olc->lostSignal() && _state.armed) {
                    _actuator->cut();
                    _state.armed = false;
                    _state.failsafe = true;
                    _board->showArmedStatus(false);
                    return;
                }

                // Check whether open-loop controller data is available
                if (!_olc->ready()) return;

                // Disarm
                if (_state.armed && !_olc->inArmedState()) {
                    _state.armed = false;
                } 

                // Avoid arming if aux1 switch down on startup
                if (!_safeToArm) {
                    _safeToArm = !_olc->inArmedState();
                }

                // Arm (after lots of safety checks!)
                if (
                        _safeToArm &&
                        !_state.armed && 
                        _olc->inactive() && 
                        _olc->inArmedState() && 
                        !_state.failsafe && 
                        _state.safeToArm()) {
                    _state.armed = true;
                }

                // Cut motors on throttle-down
                if (_state.armed && _olc->inactive()) {
                    _actuator->cut();
                }

                // Set LED based on arming status
                _board->showArmedStatus(_state.armed);

            } // checkOpenLoopController

            void startSensors(void) 
            {
                for (uint8_t k=0; k<_sensor_count; ++k) {
                    _sensors[k]->begin();
                }
            }

         public:

            void begin(bool armed=false)
            {  
                // Start the board
                _board->begin();

                // Ad-hoc debugging support
                _debugger.begin(_board);

                // Initialize state
                memset(&_state, 0, sizeof(State));

                // Initialize the sensors
                startSensors();

                // Initialize the open-loop controller
                _olc->begin();

                // Initialize safety features
                _state.failsafe = false;
                _state.armed = armed;

                // Initialize timer task for PID controllers
                _closedLoopTask.begin(_board, _olc, _actuator, &_state);

                // Initialize serial timer task
                _serialTask.begin(_board, &_state, _olc, _actuator);

                // Support safety override by simulator
                _state.armed = armed;

                // Start the actuator
                _actuator->begin();

            } // begin

            void addClosedLoopController(ClosedLoopController * controller, uint8_t modeIndex=0) 
            {
                _closedLoopTask.addClosedLoopController(controller, modeIndex);
            }

            void update(void)
            {
                // Grab control signal if available
                checkOpenLoopController();

                // Update PID controllers task
                _closedLoopTask.update();

                // Check sensors
                checkSensors();

                // Update serial comms task
                _serialTask.update();
            }
            */

        public:

            void addSensor(Sensor * sensor) 
            {
                _sensors[_sensor_count++] = sensor;
            }


    }; // class RFT

} // namespace
