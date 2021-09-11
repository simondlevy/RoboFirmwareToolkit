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
                HDR_START,
                HDR_M,
                HDR_ARROW,
                HDR_SIZE,
                PAYLOAD
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

            void prepareToSend(uint8_t type, uint8_t count, uint8_t size)
            {
                _outBufSize = 0;
                _outBufIndex = 0;
                _outBufChecksum = 0;

                addToOutBuf('$');
                addToOutBuf('M');
                addToOutBuf('>');
                serialize8(count*size);
                serialize8(type);
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

            void prepareToSendBytes(uint8_t type, uint8_t count)
            {
                prepareToSend(type, count, 1);
            }

            void sendByte(uint8_t src)
            {
                serialize8(src);
            }

            void prepareToSendShorts(uint8_t type, uint8_t count)
            {
                prepareToSend(type, count, 2);
            }

            void sendShort(short src)
            {
                int16_t a;
                memcpy(&a, &src, 2);
                serialize16(a);
            }

            void prepareToSendInts(uint8_t type, uint8_t count)
            {
                prepareToSend(type, count, 4);
            }

            void sendInt(int32_t src)
            {
                int32_t a;
                memcpy(&a, &src, 4);
                serialize32(a);
            }

            void prepareToSendFloats(uint8_t type, uint8_t count)
            {
                prepareToSend(type, count, 4);
            }

            void sendFloat(float src)
            {
                uint32_t a;
                memcpy(&a, &src, 4);
                serialize32(a);
            }

            virtual void collectPayload(uint8_t index, uint8_t value) = 0;
            virtual void dispatchMessage(uint8_t type) = 0;

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
                static uint8_t type;
                static uint8_t checksum;
                static uint8_t payload_size;
                static uint8_t payload_index;

                // Payload functions
                payload_size = parser_state == HDR_ARROW ? c : payload_size;
                payload_index = parser_state == PAYLOAD ? payload_index + 1 : 0;
                bool payload_flag = type >= 200 && parser_state == PAYLOAD;

                // Command acquisition function
                type = parser_state == HDR_SIZE ? c : type;

                // Checksum transition function
                checksum = parser_state == HDR_ARROW ? c
                    : parser_state == PAYLOAD  ?  checksum ^ c 
                    : 0;

                // Parser state transition function
                parser_state
                    = parser_state == IDLE && c == '$' ? HDR_START
                    : parser_state == HDR_START && c == 'M' ? HDR_M
                    : parser_state == HDR_M && (c == '<' || c == '>') ? HDR_ARROW
                    : parser_state == HDR_ARROW && c <= INBUF_SIZE ? HDR_SIZE
                    : parser_state == HDR_SIZE ? PAYLOAD
                    : parser_state == PAYLOAD && payload_index < payload_size ? PAYLOAD
                    : parser_state == PAYLOAD ? IDLE
                    : parser_state;

                // Payload accumulation
                if (payload_flag) {
                    collectPayload(payload_index-1, c);
                }

                // Message dispatch
                if (parser_state == IDLE && checksum == c) {
                    dispatchMessage(type);
                }

            } // parse

    }; // class Parser

} // namespace rft
