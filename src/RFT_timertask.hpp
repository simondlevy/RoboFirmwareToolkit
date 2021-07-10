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

            Board * _board = NULL;

            TimerTask(float freq)
            {
                _period = 1 / freq;
                _time = 0;
            }

            void begin(Board * board)
            {
                _board = board;
            }

            virtual void doTask(State * state) = 0;

        public:

            void update(State * state)
            {
                float time = _board->getTime();

                if ((time - _time) > _period)
                {
                    doTask(state);
                    _time = time;
                }
            }

    };  // TimerTask

} // namespace rft
