#pragma once

#include <functional>
namespace cytx
{
    class scope_guard
    {
    public:
        explicit scope_guard(std::function<void(void)> on_exit_scope_func)
            : on_exit_scope_func_(on_exit_scope_func)
            , dismissed_(false)
        {

        }

        ~scope_guard()
        {
            if (!dismissed_)
            {
                on_exit_scope_func_();
            }
        }

        //用于手动设置回滚
        void dismiss()
        {
            dismissed_ = true;
        }

    private:
        std::function<void(void)> on_exit_scope_func_;
        bool dismissed_;

    private: //noncopyable
        scope_guard(scope_guard const&) = delete;
        scope_guard& operator=(scope_guard const&) = delete;
    };
}

#define SCOPEGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line)

#define ON_SCOPE_EXIT(callback) cytx::scope_guard SCOPEGUARD_LINENAME(exit, __LINE__)([&]() callback)