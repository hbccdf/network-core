#pragma once
#include "network/meta/meta.hpp"

namespace cytx
{
    struct cmd_node
    {
        std::string cmdStr;
        bool retryWhenFailed = false;
        int retryInterval = 1000;
        int retryTimes = 1;

        int delay = 0;
        int interval = 0;
        int times = 1;

        std::string name;

        std::string nextCmdWhenFailed;
        std::string nextCmd;
    };

    REG_META(cmd_node, cmdStr, retryWhenFailed, retryInterval, retryTimes, delay, interval, times,
        name, nextCmdWhenFailed, nextCmd);

    struct cmder_config
    {
        std::string profile;
        std::unordered_map<std::string, std::string> alias;
        std::map<std::string, std::vector<cmd_node>> cmds;
    };

    REG_META(cmder_config, profile, alias, cmds);
}