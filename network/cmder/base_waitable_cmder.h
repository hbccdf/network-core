#pragma once
#include "base_cmder.h"
#include "waitable_object.h"

namespace cytx
{
    class base_waitable_cmder : public base_cmder
    {
    public:
        virtual void reset_value() override
        {
            isWait = false;
            waitTime = -1;
        }

        virtual void set_options() override
        {
            op_->add_options()
                ("isWait", "isWait")
                ("waitTime", value<int>(), "waitTime");
        }

        int execute() override
        {
            if (need_help())
            {
                show_help();
                return 0;
            }

            return process();
        }

        virtual int process() = 0;

    public:
        int wait()
        {
            return wait(waitTime);
        }

        int wait(int milliseconds)
        {
            wait_result_.wait(milliseconds);
            return get_and_clear_result();
        }

        void notify(int result)
        {
            wait_result_.set(result);
        }

    private:
        int get_and_clear_result()
        {
            int result = wait_result_.get().get_value_or(-1);
            wait_result_.reset();
            return result;
        }

    public:
        bool isWait;
        int waitTime;

    private:
        waitable<int> wait_result_;
    };

    REG_META(base_waitable_cmder, isWait, waitTime);
}