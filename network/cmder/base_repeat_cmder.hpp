﻿#pragma once
#include "base_waitable_cmder.hpp"
#include "network/util/thread.hpp"

namespace cytx
{
    class base_repeat_cmder : public base_waitable_cmder
    {
    public:
        virtual void reset_value() override
        {
            base_waitable_cmder::reset_value();

            delay = 0;
            interval = 0;
            times = 1;

            retryWhenFailed = false;
            retryInterval = 1000;
            retryTimes = 1;
        }

        virtual int execute() override
        {
            if (need_help())
            {
                show_help();
                return 0;
            }
            return repeat_execute();
        }

    private:
        int repeat_execute()
        {
            if (delay > 0)
            {
                util::sleep(delay);
            }

            int current_times = 0;
            while (current_times < times)
            {
                int ret = retry_execute();
                if (ret != 0)
                {
                    return ret;
                }

                current_times++;
                if (current_times < times && interval > 0)
                {
                    util::sleep(interval);
                }
            }

            return 0;
        }

        int retry_execute()
        {
            int ret = process();
            int current_times = 1;
            while (retryWhenFailed && ret != 0 && current_times < retryTimes)
            {
                if (retryInterval > 0)
                {
                    util::sleep(retryInterval);
                }

                ret = process();
                current_times++;
            }

            return ret;
        }

    public:
        int delay;
        int interval;
        int times;

        bool retryWhenFailed;
        int retryInterval;
        int retryTimes;
    };

    REG_METAB(base_repeat_cmder, base_waitable_cmder, delay, interval, times,
        retryWhenFailed, retryInterval, retryTimes);
}