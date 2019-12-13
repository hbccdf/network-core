#pragma once
#include "base_cmder.hpp"
#include "icmder_manager.hpp"
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
        void after_process() override
        {
            wait_result_.reset();
        }

        int wait()
        {
            return wait(waitTime);
        }

        int wait(int milliseconds)
        {
            set_waiting_cmder();

            int result = wait_result_.get(milliseconds).get_value_or(-1);
            wait_result_.reset();

            clear_waiting_cmder();

            return result;
        }

        void notify(int result)
        {
            wait_result_.set(result);
        }

        void notify_waiting_result(int result) override
        {
            notify(result);
        }

    private:
        void set_waiting_cmder()
        {
            if (need_register_waiting_cmder_)
                manager_->set_waiting_cmder(this);
        }

        void clear_waiting_cmder()
        {
            if (need_register_waiting_cmder_)
                manager_->clear_waiting_cmder();
        }

    public:
        bool isWait;
        int waitTime;

    protected:
        bool need_register_waiting_cmder_ = true;

    private:
        waitable<int> wait_result_;
    };

    REG_META(base_waitable_cmder, isWait, waitTime);
}