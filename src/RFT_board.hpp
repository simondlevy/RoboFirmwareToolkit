/*
   Class header for board-specific routines

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include <stdarg.h>
#include <stdint.h>

namespace rft {

    class Board {

        friend class RFT;
        friend class Debugger;
        friend class TimerTask;
        friend class SerialTask;
        friend class ClosedLoopTask;

        protected:

            //------------------------------------ Core functionality ----------------------------------------------------
            virtual float getTime(void) = 0;

            //-------------------------------------- For real boards -----------------------------------------------------
            virtual void begin(void) { }

            //------------------------------- Serial communications via MSP ----------------------------------------------
            virtual uint8_t serialAvailable(void) { return 0; }
            virtual uint8_t serialRead(void)  { return 1; }
            virtual void    serialWrite(uint8_t c) { (void)c; }

            //----------------------------------------- Safety -----------------------------------------------------------
            virtual void showArmedStatus(bool armed) { (void)armed; }
            virtual void flashLed(bool shouldflash) { (void)shouldflash; }

            //--------------------------------------- Debugging ----------------------------------------------------------
            static  void outbuf(char * buf);

    }; // class Board

} // namespace
