/*
   PID controller for a single degree of freedom.

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include "RFT_filters.hpp"
#include "RFT_closedloop.hpp"

namespace rft {

    // Because time differences (dt) appear more-or-less constant, we avoid
    // incoroporating them into the code; i.e., they are "absorbed" into tuning
    // constants Ki and Kd.
    class DofPid {

        private: 

            // PID constants
            float _Kp = 0;
            float _Ki = 0;
            float _Kd = 0;

            // Accumulated values
            float _lastError   = 0;
            float _errorI      = 0;
            float _deltaError1 = 0;
            float _deltaError2 = 0;

            // For deltaT-based controllers
            float _previousTime = 0;
     
            // Prevents integral windup
            float _windupMax = 0;

        protected:

            DofPid(const float Kp,
                   const float Ki,
                   const float Kd,
                   const float windupMax=0.4) 
            {
                // Set constants
                _Kp = Kp;
                _Ki = Ki;
                _Kd = Kd;
                _windupMax = windupMax;

                // Initialize error integral, previous value
                reset();
            }

            float compute(float target, float actual)
            {
                // Compute error as scaled target minus actual
                float error = target - actual;

                // Compute P term
                float pterm = error * _Kp;

                // Compute I term
                float iterm = 0;
                if (_Ki > 0) { // optimization
                    _errorI = Filter::constrainAbs(_errorI + error, _windupMax);
                    iterm =  _errorI * _Ki;
                }

                // Compute D term
                float dterm = 0;
                if (_Kd > 0) { // optimization
                    float deltaError = error - _lastError;
                    dterm = (_deltaError1 + _deltaError2 + deltaError) * _Kd; 
                    _deltaError2 = _deltaError1;
                    _deltaError1 = deltaError;
                    _lastError = error;
                }

                return pterm + iterm + dterm;
            }

            void resetOnInactivity(bool inactive)
            {
                if (inactive) {
                    reset();
                }
            }

            void reset(void)
            {
                _errorI = 0;
                _lastError = 0;
                _previousTime = 0;
            }

    };  // class DofPid

} // namespace rft
