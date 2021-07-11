/*
   PID controller for angular velocity

   Copyright (c) 2021 Simon D. Levy

   MIT License
 */

#pragma once

#include <rft_closedloops/dofpid.hpp>

namespace rft {

    class AngularVelocityPid : public rft::DofPid {

        private: 

            // Arbitrary constants
            static constexpr float BIG_DEGREES_PER_SECOND = 40.0f; 
            static constexpr float WINDUP_MAX = 6.0f;

            // Converted to radians from degrees in constructor for efficiency
            float _bigAngularVelocity = 0;

        public:

            void init(const float Kp, const float Ki, const float Kd) 
            {
                DofPid::init(Kp, Ki, Kd, WINDUP_MAX);

                // Convert degree parameters to radians for use later
                _bigAngularVelocity =
                    rft::Filter::deg2rad(BIG_DEGREES_PER_SECOND);
            }

            float compute(float demand, float angularVelocity)
            {
                // Reset integral on quick angular velocity change
                if (fabs(angularVelocity) > _bigAngularVelocity) {
                    reset();
                }

                return DofPid::compute(demand, angularVelocity);
            }

    };  // class AngularVelocityPid

} // namespace rft
