/*
   Core class for RoboFirmwareToolkit

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
#include "RFT_closedlooptask.hpp"

namespace rft {

    class RFTPure {

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

            void checkOpenLoopController(Board * board,
                                         OpenLoopController * olc,
                                         Actuator * actuator,
                                         State * state)
            {
                // Sync failsafe to open-loop-controller
                if (olc->lostSignal() && state->armed) {
                    actuator->cut();
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
                    actuator->cut();
                }

                // Set LED based on arming status
                board->showArmedStatus(state->armed);

            } // checkOpenLoopController

        protected:

            RFTPure(void)
            {
                // Support adding new sensors
                _sensor_count = 0;
            }

            void begin(Board * board,
                       OpenLoopController * olc,
                       Actuator * actuator)
            {  
                // Start the board
                board->begin();

                // Initialize the sensors
                startSensors();

                // Initialize the open-loop controller
                olc->begin();

                // Start the actuator
                actuator->begin();

            } // begin

            static void begin(
                    Board * board,
                    OpenLoopController * olc,
                    Actuator * actuator,
                    Sensor ** sensors,
                    uint8_t sensor_count)
            {  
                // Start the board
                board->begin();

                // Initialize the sensors
                for (uint8_t k=0; k<sensor_count; ++k) {
                    sensors[k]->begin();
                }

                // Initialize the open-loop controller
                olc->begin();

                // Start the actuator
                actuator->begin();

            } // begin

            void update(Board * board,
                        OpenLoopController * olc,
                        Actuator * actuator,
                        State * state)
            {
                // Grab control signal if available
                checkOpenLoopController(board, olc, actuator, state);

                // Update PID controllers task
                _closedLoopTask.update(board, olc, actuator, state);

                // Check sensors
                checkSensors(board, state);
            }

        public:

            void addSensor(Sensor * sensor) 
            {
                _sensors[_sensor_count++] = sensor;
            }

            void addClosedLoopController(ClosedLoopController * controller,
                                         uint8_t modeIndex=0) 
            {
                _closedLoopTask.addController(controller, modeIndex);
            }

    }; // class RFTPure

} // namespace rft
