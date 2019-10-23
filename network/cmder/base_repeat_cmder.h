#pragma once
#include "base_waitable_cmder.h"
#include <thread>

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


        virtual void set_options() override
        {
            base_waitable_cmder::set_options();

            op_->add_options()
                ("delay", value<int>(), "")
                ("interval", value<int>(), "")
                ("times", value<int>(), "")
                ("retryWhenFailed", "")
                ("retryInterval", value<int>(), "")
                ("retryTimes", value<int>(), "");
        }


        virtual int execute() override
        {
            return repeat_execute();
        }

    private:
        int repeat_execute()
        {
            if (delay > 0)
            {
                sleep(delay);
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
                    sleep(interval);
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
                    sleep(retryInterval);
                }

                ret = process();
                current_times++;
            }

            return ret;
        }

        void sleep(int milliseconds)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
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