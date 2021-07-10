/*
   Abstract class for timer tasks

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "RFT_board.hpp"
#include "RFT_debugger.hpp"
#include "RFT_state.hpp"

namespace rft {

    class TimerTask {

        private:

            float _period = 0;
            float _time = 0;

        protected:

            TimerTask(float freq)
            {
                _period = 1 / freq;
                _time = 0;
            }

            virtual void doTask(Board * board, OpenLoopController * olc, Actuator * actuator, State * state) = 0;

        public:

            void update(Board * board, OpenLoopController * olc, Actuator * actuator, State * state)
            {
                float time = board->getTime();

                if ((time - _time) > _period)
                {
                    doTask(board, olc, actuator, state);
                    _time = time;
                }
            }

    };  // TimerTask

} // namespace rft
