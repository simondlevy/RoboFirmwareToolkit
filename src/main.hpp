/*
   Main class for RoboFirmwareToolkit

   Copyright (c) 2021 Simon D. Levy

   MIT License
*/

#pragma once

#include "debugger.hpp"
#include "mspparser.hpp"
#include "board.hpp"
#include "openloop.hpp"
#include "states/mavstate.hpp"
#include "closedloop.hpp"
#include "sensor.hpp"
#include "actuator.hpp"
#include "timertasks/closedlooptask.hpp"
#include "timertasks/serialtask.hpp"

namespace rft {

    class Main {

        private:

            static constexpr float MAX_ARMING_ANGLE_DEGREES = 25.0f;

            // Supports periodic ad-hoc debugging
            Debugger _debugger;

            // Sensors 
            Sensor * _sensors[256] = {NULL};
            uint8_t _sensor_count = 0;

            // Safety
            bool _safeToArm = false;

            // Timer task for PID controllers
            ClosedLoopTask _closedLoopTask;

            // Passed to Main::begin() for a particular build
            Actuator * _actuator = NULL;

            // Serial timer task for GCS
            SerialTask _serialTask;

            Board * _board = NULL;
            OpenLoopController * _olc = NULL;

            // Vehicle state
            MavState _state;

            void checkSensors(void)
            {
                static uint32_t counts[2];
                /*
                static uint32_t start;
                uint32_t time = millis();
                if (time-start > 1000) {
                    Debugger::printf("q=%d  g=%d\n", counts[0], counts[1]);
                    counts[0] = 0;
                    counts[1] = 0;
                    start = time;
                }*/

                for (uint8_t k=0; k<_sensor_count; ++k) {
                    Sensor * sensor = _sensors[k];
                    float time = _board->getTime();
                    if (sensor->ready(time)) {
                        sensor->modifyState(&_state, time);
                        if (k<2) counts[k]++;
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

            Main(Board * board, OpenLoopController * olc, Actuator * actuator)
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

            void addSensor(Sensor * sensor) 
            {
                _sensors[_sensor_count++] = sensor;
            }

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

    }; // class Main

} // namespace
