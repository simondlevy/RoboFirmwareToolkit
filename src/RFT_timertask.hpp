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

            bool ready(Board * board)
            {
                float time = board->getTime();

                if ((time - _time) > _period)
                {
                    _time = time;
                    return true;
                }

                return false;
             }

    };  // TimerTask

} // namespace rft
