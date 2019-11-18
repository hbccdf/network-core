#pragma once
#include "base_cmder.hpp"
#include "network/base/waitable_object.hpp"

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

    public:
        int wait()
        {
            return wait(waitTime);
        }

        int wait(int milliseconds)
        {
            int result = wait_result_.get(milliseconds).get_value_or(-1);
            wait_result_.reset();
            return result;
        }

        void notify(int result)
        {
            wait_result_.set(result);
        }

    public:
        bool isWait;
        int waitTime;

    private:
        waitable<int> wait_result_;
    };

    REG_META(base_waitable_cmder, isWait, waitTime);
}