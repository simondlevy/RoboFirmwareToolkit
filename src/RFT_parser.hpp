/*
   MSP parsing routines

   Copyright (C) Simon D. Levy 2021

   MIT License
 */


#pragma once

#include <stdint.h>
#include <string.h>

namespace rft {

    class Parser {

        private:

            static const uint8_t MAXMSG = 255;

            static const int INBUF_SIZE  = 128;
            static const int OUTBUF_SIZE = 128;

            typedef enum serialState_t {
                IDLE,
                HEADER_START,
                HEADER_M,
                HEADER_ARROW,
                HEADER_SIZE,
                HEADER_CMD
            } serialState_t;


            uint8_t _outBufChecksum;
            uint8_t _outBuf[OUTBUF_SIZE];
            uint8_t _outBufIndex;
            uint8_t _outBufSize;

            void serialize16(int16_t a)
            {
                serialize8(a & 0xFF);
                serialize8((a >> 8) & 0xFF);
            }

            void serialize32(uint32_t a)
            {
                serialize8(a & 0xFF);
                serialize8((a >> 8) & 0xFF);
                serialize8((a >> 16) & 0xFF);
                serialize8((a >> 24) & 0xFF);
            }

            void prepareToSend(uint8_t command, uint8_t count, uint8_t size)
            {
                _outBufSize = 0;
                _outBufIndex = 0;
                _outBufChecksum = 0;

                addToOutBuf('$');
                addToOutBuf('M');
                addToOutBuf('>');
                serialize8(count*size);
                serialize8(command);
            }

            static uint8_t CRC8(uint8_t * data, int n) 
            {
                uint8_t crc = 0x00;

                for (int k=0; k<n; ++k) {

                    crc ^= data[k];
                }

                return crc;
            }

            void addToOutBuf(uint8_t a)
            {
                _outBuf[_outBufSize++] = a;
            }

        protected:

            void completeSend(void)
            {
                serialize8(_outBufChecksum);
            }

            void serialize8(uint8_t a)
            {
                addToOutBuf(a);
                _outBufChecksum ^= a;
            }

            void prepareToSendBytes(uint8_t command, uint8_t count)
            {
                prepareToSend(command, count, 1);
            }

            void sendByte(uint8_t src)
            {
                serialize8(src);
            }

            void prepareToSendShorts(uint8_t command, uint8_t count)
            {
                prepareToSend(command, count, 2);
            }

            void sendShort(short src)
            {
                int16_t a;
                memcpy(&a, &src, 2);
                serialize16(a);
            }

            void prepareToSendInts(uint8_t command, uint8_t count)
            {
                prepareToSend(command, count, 4);
            }

            void sendInt(int32_t src)
            {
                int32_t a;
                memcpy(&a, &src, 4);
                serialize32(a);
            }

            void prepareToSendFloats(uint8_t command, uint8_t count)
            {
                prepareToSend(command, count, 4);
            }

            void sendFloat(float src)
            {
                uint32_t a;
                memcpy(&a, &src, 4);
                serialize32(a);
            }

            virtual void setInputBuffer(uint8_t index, uint8_t value) = 0;
            virtual void dispatchMessage(uint8_t command) = 0;

            void begin(void)
            {
                _outBufChecksum = 0;
                _outBufIndex = 0;
                _outBufSize = 0;
            }

            uint8_t availableBytes(void)
            {
                return _outBufSize;
            }

            uint8_t readByte(void)
            {
                _outBufSize--;
                return _outBuf[_outBufIndex++];
            }

            void parse(uint8_t c)
            {
                static serialState_t parser_state;

                static uint8_t command;
                static uint8_t checksum_in;
                static uint8_t dataSize;
                static uint8_t inBufOffset;

                // Input buffer transition function
                inBufOffset = parser_state == IDLE ? 0 : inBufOffset;

                // Data size acquisition function
                dataSize = parser_state == HEADER_ARROW ? c : dataSize;

                // Command acquisition function
                command = parser_state == HEADER_SIZE ? c : command;

                // Checksum transition function
                checksum_in = parser_state == HEADER_ARROW ? c
                    : parser_state == HEADER_SIZE ? checksum_in ^ c 
                    : parser_state == HEADER_CMD && inBufOffset < dataSize ? checksum_in ^ c : checksum_in;

                // Message dispatch
                if (parser_state == HEADER_CMD && command < 200) {
                    if (checksum_in == c) {
                       dispatchMessage(command);
                    }
                }

                // Always set the input buffer
                setInputBuffer(inBufOffset, c);

                // Parser state transition function
                switch (parser_state) {

                    case IDLE:
                        parser_state = (c == '$') ? HEADER_START : IDLE;
                        break;

                    case HEADER_START:
                        parser_state = (c == 'M') ? HEADER_M : IDLE;
                        break;

                    case HEADER_M:
                        switch (c) {
                            case '>':
                            case '<':
                                parser_state = HEADER_ARROW;
                                break;
                            default:
                                parser_state = IDLE;
                        }
                        break;

                    case HEADER_ARROW:
                        // now we are expecting the payload size
                        if (c > INBUF_SIZE) {       
                            parser_state = IDLE;
                            break;
                        }

                        // the command is to follow
                        parser_state = HEADER_SIZE;      
                        break;

                    case HEADER_SIZE:
                        parser_state = HEADER_CMD;
                        break;

                    case HEADER_CMD:

                        // a command to set something like motors
                        if (command >= 200) {

                            if (inBufOffset < dataSize) {
                                inBufOffset++;
                            }

                            else {
                                if (checksum_in == c) {
                                    dispatchMessage(command);
                                }
                                parser_state = IDLE;
                            }
                        }

                        else {
                            parser_state = IDLE;
                        }

                } // switch (parser_state)

            } // parse

    }; // class Parser

} // namespace rft
