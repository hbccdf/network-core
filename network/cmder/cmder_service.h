#pragma once
#include "icmder_manager.h"
#include "network/service/base_service.hpp"
#include "network/service/service_meta.hpp"

namespace cytx
{
    class cmder_service : public base_service
    {
    public:
        void init()
        {
            manager_ = world_ptr_->get<icmder_manager>("icmder_mgr");
        }

    public:
        int handle_input(std::string input)
        {
            return manager_->handle_input(input);
        }

        int handle_input(int argc, char* argv[])
        {
            return manager_->handle_input(argc, argv);
        }

        void show_help()
        {
            manager_->dump_help();
        }

        void show_cmder_help(const std::string& cmd_name)
        {
            auto cmder_ptr = manager_->get_cmder(cmd_name);
            if (cmder_ptr)
            {
                cmder_ptr->dump_help();
            }
            else
            {
                std::cout << fmt::format("not find cmder {}", cmd_name) << std::endl;
                show_help();
            }
        }

    private:
        icmder_manager* manager_ = nullptr;
    };

    REG_SERVICE(cmder_service);
}