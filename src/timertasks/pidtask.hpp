/*
   Timer task for PID controllers

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "timertask.hpp"
#include "state.hpp"
#include "demands.hpp"

namespace rft {

    class PidTask : public TimerTask {

        friend class Toolkit;

        private:

            // For now, we keep all PID timer tasks the same.  At some point it might be useful to 
            // investigate, e.g., faster updates for Rate PID than for Level PID.
            static constexpr float FREQ = 300;

            // PID controllers
            PidController * _pid_controllers[256] = {NULL};
            uint8_t _pid_controller_count = 0;

            // Other stuff we need
            OpenLoopController * _olc = NULL;
            Actuator * _actuator = NULL;
            State  * _state    = NULL;

        protected:

            PidTask(void)
                : TimerTask(FREQ)
            {
                _pid_controller_count = 0;
            }

            void begin(Board * board, OpenLoopController * olc, Actuator * actuator, State * state)
            {
                TimerTask::begin(board);

                _olc = olc;
                _actuator = actuator;
                _state = state;
            }

            void addPidController(PidController * pidController, uint8_t modeIndex) 
            {
                pidController->modeIndex = modeIndex;

                _pid_controllers[_pid_controller_count++] = pidController;
            }

            virtual void doTask(void) override
            {
                // Start with demands from open-loop controller
                demands_t demands = {};
                _olc->getDemands(demands);

                // Each PID controllers is associated with at least one auxiliary switch state
                uint8_t modeIndex = _olc->getModeIndex();

                // Some PID controllers should cause LED to flash when they're active
                bool shouldFlash = false;

                for (uint8_t k=0; k<_pid_controller_count; ++k) {

                    PidController * pidController = _pid_controllers[k];

                    // Some PID controllers need to reset their integral based on inactivty (e.g., throttle down)
                    pidController->resetOnInactivity(_olc->inactive());

                    if (pidController->modeIndex <= modeIndex) {

                        pidController->modifyDemands(_state, demands); 

                        if (pidController->shouldFlashLed()) {
                            shouldFlash = true;
                        }
                    }
                }

                // Flash LED for certain PID controllers
                _board->flashLed(shouldFlash);

                // Use updated demands to run motors
                if (_state->armed && !_state->failsafe && !_olc->inactive()) {
                    _actuator->run(demands);
                }
             }

    };  // PidTask

} // namespace rft
