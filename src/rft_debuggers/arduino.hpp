/*
   Arduino debugging support

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "rft_debugger.hpp"

namespace rft {

    void Debugger::outbuf(char * buf)
    {
        Serial.print(buf);
    }

} // namespace rft
