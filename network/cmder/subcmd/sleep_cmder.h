#pragma once
#include "network/cmder/base_cmder.h"
#include <thread>

namespace cytx
{
    namespace subcmd
    {
        class sleep_cmder : public base_cmder
        {
        public:
            void reset_value() override
            {
                millisecond = 0;
            }

            int process() override
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(millisecond));
                return 0;
            }

        public:
            int millisecond;
        };

        REG_META(sleep_cmder, millisecond);

        REG_CMDER(sleep_cmder, "sleep", "sleep some millisecond time")("millisecond,m", value<int>(), "Millisecond").end();
    }
}