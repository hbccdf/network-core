#pragma once
#include "network/cmder/base_cmder.h"
#include "network/util/string.hpp"

namespace cytx
{
    namespace subcmd
    {
        class wait_cmder : public base_cmder
        {
        public:
            void reset_value()
            {
                input = "";
            }

            int process() override
            {
                std::string input_str;
                std::cin >> input_str;
                string_util::trim(input_str);
                string_util::trim(input);
                if (input_str.empty() || input.empty() || input_str == input)
                    return 0;

                return 1;
            }

        public:
            std::string input;
        };

        REG_META(wait_cmder, input);
        REG_CMDER(wait_cmder, "wait", "wait input")
            ("input,i", value<std::string>(), "waiting input content").end();
    }
}