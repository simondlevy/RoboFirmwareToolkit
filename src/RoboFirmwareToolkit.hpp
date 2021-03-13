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

            // Safety
            bool _safeToArm = false;

            // Sensors 
            Sensor * _sensors[256] = {NULL};
            uint8_t _sensor_count = 0;

            // Timer task for PID controllers
            ClosedLoopTask _closedLoopTask;

            RFT(Board * board, OpenLoopController * olc, Actuator * actuator)
            {
                // Store the essentials
                _board    = board;
                _olc = olc;
                _actuator = actuator;

                // Support adding new sensors
                _sensor_count = 0;
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

            } // begin

            virtual State * getState(void) = 0;

            void checkSensors(void)
            {
                State * state = getState();

                for (uint8_t k=0; k<_sensor_count; ++k) {
                    rft::Sensor * sensor = _sensors[k];
                    float time = _board->getTime();
                    if (sensor->ready(time)) {
                        sensor->modifyState(state, time);
                    }
                }
            }

            void checkOpenLoopController(void)
            {
                State * state = getState();

                // Sync failsafe to open-loop-controller
                if (_olc->lostSignal() && state->armed) {
                    _actuator->cut();
                    state->armed = false;
                    state->failsafe = true;
                    _board->showArmedStatus(false);
                    return;
                }

                // Check whether open-loop controller data is available
                if (!_olc->ready()) return;

                // Disarm
                if (state->armed && !_olc->inArmedState()) {
                    state->armed = false;
                } 

                // Avoid arming if aux1 switch down on startup
                if (!_safeToArm) {
                    _safeToArm = !_olc->inArmedState();
                }

                // Arm (after lots of safety checks!)
                if (
                        _safeToArm &&
                        !state->armed && 
                        _olc->inactive() && 
                        _olc->inArmedState() && 
                        !state->failsafe && 
                        state->safeToArm()) {
                    state->armed = true;
                }

                // Cut motors on throttle-down
                if (state->armed && _olc->inactive()) {
                    _actuator->cut();
                }

                // Set LED based on arming status
                _board->showArmedStatus(state->armed);

            } // checkOpenLoopController
 private:

            void startSensors(void) 
            {
                for (uint8_t k=0; k<_sensor_count; ++k) {
                    _sensors[k]->begin();
                }
            }


        public:

            void addSensor(Sensor * sensor) 
            {
                _sensors[_sensor_count++] = sensor;
            }

            void addClosedLoopController(rft::ClosedLoopController * controller, uint8_t modeIndex=0) 
            {
                _closedLoopTask.addClosedLoopController(controller, modeIndex);
            }


    }; // class RFT

} // namespace
