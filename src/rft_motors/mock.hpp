/*
   Mock implementation of motors for testing and simulation

   Copyright(C) 2021 Simon D.Levy

   MIT License
   */

#pragma once

namespace rft {

    class MockMotor {

        private:

            float _value = 0;

        protected:

            void write(float value)
            {
                _value = value;
            }

        public:

            float getValue(void)
            {
                return _value;
            }

    }; // class MockMotor

} // namespace rft
