#pragma once
#include "network/cmder/cmder_meta.hpp"
#include "network/util/string.hpp"
#include "network/base/log.hpp"

namespace cytx
{
    namespace subcmd
    {
        class echo_cmder : public base_cmder
        {
        public:
            void reset_value()
            {
                str = "";
                level = log_level_t::debug;
            }

            int process() override
            {
                LOGGER_LOG(level, str);
                return 0;
            }

        public:
            std::string str;
            log_level_t level;
        };

        REG_META(echo_cmder, str, level);
        REG_CMDER(echo_cmder, "echo", "echo msg")
            ("str,s", "content to echo")
            ("level,l", "msg level, trace, debug, info, warn, err").end();
    }
}