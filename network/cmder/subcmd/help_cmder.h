﻿#pragma once
#include "network/cmder/base_cmder.h"
#include "network/cmder/cmder_service.h"

namespace cytx
{
    namespace subcmd
    {
        class help_cmder : public base_cmder
        {
        public:
            void init() override
            {
                service_manager* service_mgr = world_ptr_->get<service_manager>("service_mgr");

                cmder_service_ = service_mgr->get_service<cmder_service>();
            }

            void reset_value() override
            {
                command = "";
            }

            int execute() override
            {
                boost::trim(command);
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
        REGISTER_CMDER(help_cmder, "help", "show help info")
            ("command", value<std::string>(), "command", -1).end();
    }
}