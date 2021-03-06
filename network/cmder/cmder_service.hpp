﻿#pragma once
#include "icmder_manager.hpp"
#include "network/service/base_service.hpp"
#include "network/service/service_meta.hpp"

namespace cytx
{
    class cmder_service : public base_service
    {
    public:
        bool init()
        {
            manager_ = world_ptr_["icmder_mgr"];
            return true;
        }

    public:
        int handle_input(const std::string& input)
        {
            return manager_->handle_input(input);
        }

        int handle_input(int argc, char* argv[])
        {
            return manager_->handle_input(argc, argv);
        }

        void show_help() const
        {
            manager_->show_help();
        }

        void show_cmder_help(const std::string& cmd_name) const
        {
            auto cmder_ptr = manager_->get_cmder(cmd_name);
            if (cmder_ptr)
            {
                cmder_ptr->show_help();
            }
            else
            {
                std::cout << fmt::format("not find cmder {}", cmd_name) << std::endl;
                show_help();
            }
        }

        void register_alias(const std::unordered_map<std::string, std::string>& alias)
        {
            manager_->register_alias(alias);
        }

        const std::unordered_map<std::string, std::string>& get_alias() const
        {
            return manager_->get_alias();
        }

    private:
        icmder_manager* manager_ = nullptr;
    };

    REG_SERVICE(cmder_service);
}