#pragma once
#include "network/meta/meta.hpp"

namespace cytx
{
    struct cmd_node
    {
        std::string cmdStr;
        bool retryWhenFailed;
        int retryInterval;
        int retryTimes;

        int delay;
        int interval;
        int times;

        std::string name;

        std::string nextCmdWhenFailed;
        std::string nextCmd;
    };

    REG_META(cmd_node, cmdStr, retryWhenFailed, retryInterval, retryTimes, delay, interval, times,
        name, nextCmdWhenFailed, nextCmd);

    struct cmder_config
    {
        std::string profile;
        std::map<std::string, std::vector<cmd_node>> cmds;
    };

    REG_META(cmder_config, profile, cmds);
}