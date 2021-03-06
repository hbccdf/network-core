﻿#pragma once
#include "network/cmder/cmder_meta.hpp"
#include "network/cmder/cmder_service.hpp"

namespace cytx
{
    namespace subcmd
    {
        class interactive_cmder : public base_cmder
        {
        public:
            void init() override
            {
                service_manager* service_mgr = world_ptr_["service_mgr"];
                cmder_service_ = service_mgr->get();
            }

            void reset_value() override
            {
                prompt = "fc>";
                exit = "exit";
            }

            int process() override
            {
                std::string input;
                while (true)
                {
                    try
                    {
                        read_input(input);
                        string_util::trim(input);
                        if (input.empty())
                            continue;

                        if (input == exit)
                            break;

                        cmder_service_->handle_input(input);
                    }
                    catch (std::exception& e)
                    {
                        std::cout << "execute error: " << e.what() << std::endl;
                    }
                }

                return 0;
            }

        private:
            void read_input(std::string& input)
            {
                std::cout << prompt;
                std::getline(std::cin, input);
            }

        public:
            std::string prompt;
            std::string exit;

        private:
            cmder_service* cmder_service_;
        };

        REG_META(interactive_cmder, prompt, exit);
        REG_CMDER(interactive_cmder, "interactive", "interactive mode")
            ("prompt", "prompt text, default is fc>")
            ("exit", "exit when input equal the value").end();
    }
}