﻿#pragma once
#include "network/cmder/cmder_meta.hpp"
#include "network/cmder/cmder_service.hpp"

namespace cytx
{
    namespace subcmd
    {
        class help_cmder : public base_cmder
        {
        public:
            void init() override
            {
                service_manager* service_mgr = world_ptr_["service_mgr"];
                cmder_service_ = service_mgr->get();
            }

            void reset_value() override
            {
                command = "";
            }

            int process() override
            {
                string_util::trim(command);
                if (command.empty())
                {
                    cmder_service_->show_help();
                }
                else
                {
                    cmder_service_->show_cmder_help(command);
                }

                return 0;
            }

        public:
            std::string command;

        private:
            cmder_service* cmder_service_;
        };

        REG_META(help_cmder, command);
        REG_CMDER(help_cmder, "help", "show help info")
            ("command", "command", -1).end();
    }
}