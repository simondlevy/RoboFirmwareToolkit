/*
   RFT class supporting serial communication

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "RFT_pure.hpp"
#include "RFT_serialtask.hpp"

namespace rft {

    class RFT : public RFTPure {

        private:

            // Serial tasks
            SerialTask * _serial_tasks[10] = {};
            uint8_t _serial_task_count = 0;

        protected:

            RFT(void)
                : RFTPure()
            {
                _serial_task_count = 0;
            }

            void update(Board * board, OpenLoopController * olc, Actuator * actuator, State * state)
            {
                RFTPure::update(board, olc, actuator, state);

                // Update serial tasks
                for (uint8_t k=0; k<_serial_task_count; ++k) {
                    _serial_tasks[k]->update(board, actuator, state);
                }
            }

            void addSerialTask(SerialTask * task)
            {
                _serial_tasks[_serial_task_count++] = task;
            }

    }; // class RFT

} // namespace