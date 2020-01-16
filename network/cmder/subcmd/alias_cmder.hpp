#pragma once
#include "network/cmder/cmder_meta.hpp"
#include "network/cmder/cmder_service.hpp"

namespace cytx
{
    namespace subcmd
    {
        class alias_cmder : public base_cmder
        {
        public:
            void init() override
            {
                service_manager* service_mgr = world_ptr_["service_mgr"];
                cmder_service_ = service_mgr->get();
            }

            void reset_value() override
            {
                commands.clear();
            }

            int process() override
            {
                auto& alias = cmder_service_->get_alias();
                if (commands.empty())
                {
                    for (auto& p : alias)
                    {
                        std::cout << fmt::format("{} = {}", p.first, p.second) << std::endl;
                    }
                    return 0;
                }
                for (auto& cmd : commands)
                {
                    auto it = alias.find(cmd);
                    if (it == alias.end())
                    {
                        std::cout << fmt::format("not find alias: {}", cmd) << std::endl;
                    }
                    else
                    {
                        std::cout << fmt::format("{} = {}", cmd, it->second) << std::endl;
                    }
                }
                return 0;
            }

        public:
            std::vector<std::string> commands;

        private:
            cmder_service* cmder_service_;
        };

        REG_META(alias_cmder, commands);
        REG_CMDER(alias_cmder, "alias", "show alias info")
            ("commands", "commands", -1).end();
    }
}