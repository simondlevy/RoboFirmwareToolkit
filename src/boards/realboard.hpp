/*
   Board subclass for real (hardware) boards

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "board.hpp"
#include "debugger.hpp"

namespace rft {

    class RealBoard : public Board {

        private:

            static constexpr float   LED_STARTUP_FLASH_SECONDS = 1.0;
            static constexpr uint8_t LED_STARTUP_FLASH_COUNT   = 20;
            static constexpr float   LED_SLOWFLASH_SECONDS     = 0.25;

            bool _shouldFlash = false;

            // Supports MSP over wireless protcols like Bluetooth
            bool _useSerialTelemetry = false;

            float _rollAdjustRadians = 0;
            float _pitchAdjustRadians = 0;

        protected:

            virtual void setLed(bool isOn) = 0;

            void begin(void)
            {
                // Flash LED
                float pauseSeconds = LED_STARTUP_FLASH_SECONDS / LED_STARTUP_FLASH_COUNT;
                setLed(false);
                for (uint8_t i = 0; i < LED_STARTUP_FLASH_COUNT; i++) {
                    setLed(true);
                    delaySeconds(pauseSeconds);
                    setLed(false);
                    delaySeconds(pauseSeconds);
                }
                setLed(false);

                _shouldFlash = false;
            }

            float getTime(void)
            {
                return micros() / 1.e6f;
            }

            void delaySeconds(float sec)
            {
                delay((uint32_t)(1000*sec));
            }

            uint8_t serialAvailableBytes(void)
            {
                // Attempt to use telemetry first
                if (serialTelemetryAvailable()) {
                    _useSerialTelemetry = true;
                    return serialTelemetryAvailable();
                }

                // Default to USB
                if (serialNormalAvailable() > 0) {
                    _useSerialTelemetry = false;
                    return serialNormalAvailable();
                }

                return 0;
            }

            uint8_t serialReadByte(void)
            {
                return _useSerialTelemetry? serialTelemetryRead() : serialNormalRead();
            }

            void serialWriteByte(uint8_t c)
            {
                if (_useSerialTelemetry) {
                    serialTelemetryWrite(c);
                }
                else {
                    serialNormalWrite(c);
                }
            }

            virtual uint8_t serialNormalAvailable(void) = 0;

            virtual uint8_t serialNormalRead(void) = 0;

            virtual void    serialNormalWrite(uint8_t c) = 0;

            virtual uint8_t serialTelemetryAvailable(void)
            {
                return 0;
            }

            virtual uint8_t serialTelemetryRead(void)
            {
                return 0;
            }

            virtual void serialTelemetryWrite(uint8_t c)
            {
                (void)c;
            }

            void showArmedStatus(bool armed)
            {
                // Set LED to indicate armed
                if (!_shouldFlash) {
                    setLed(armed);
                }
            }

            void flashLed(bool shouldflash)
            {
                if (shouldflash) {

                    static float _time;
                    static bool state;

                    float time = getTime();

                    if (time-_time > LED_SLOWFLASH_SECONDS) {
                        state = !state;
                        setLed(state);
                        _time = time;
                    }
                }

                _shouldFlash = shouldflash;
            }

            void error(const char * errmsg) 
            {
                while (true) {
                    Debugger::printf("%s\n", errmsg);
                    delaySeconds(0.1);
                }
            }
 
        public:

            /**
             * Compensates for poorly-mounted IMU.
             */
            void setRollAndPitchOffsets(float rollDegrees, float pitchDegrees)
            {
                _rollAdjustRadians  = Filter::deg2rad(rollDegrees);
                _pitchAdjustRadians = Filter::deg2rad(pitchDegrees);
            }

    }; // class RealBoard

} // namespace rft
