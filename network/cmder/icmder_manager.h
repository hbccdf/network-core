#pragma once
#include "base_cmder.h"
#include "network/base/world.hpp"
#include <algorithm>

namespace cytx
{
    using base_cmder_ptr = base_cmder*;
    class icmder_manager
    {
    public:
        static icmder_manager& ins()
        {
            static icmder_manager manager;
            return manager;
        }
    public:
        void set_world(world_ptr_t world_ptr)
        {
            world_ptr_ = world_ptr;

            for (auto& p : cmders_)
            {
                p.second->set_world(world_ptr_);
            }
        }

        void init_all_cmder()
        {
            for (auto& p : cmders_)
            {
                p.second->init();
            }
        }

        void register_cmder(std::string cmd_name, base_cmder_ptr cmder)
        {
            cmders_[cmd_name] = cmder;
            int cmd_name_size = (int)cmd_name.size();
            if (cmder_max_length_ < cmd_name_size)
            {
                cmder_max_length_ = cmd_name_size;
            }
        }

        void register_cmder(base_cmder_ptr cmder)
        {
            register_cmder(cmder->name(), cmder);
        }

        base_cmder_ptr get_cmder(std::string cmd_name) const
        {
            auto it = cmders_.find(cmd_name);
            if (it != cmders_.end())
            {
                return it->second;
            }

            return nullptr;
        }

        int handle_input(std::string input)
        {
            std::vector<std::string> strs;
            boost::split(strs, input, boost::is_any_of(" "), boost::token_compress_on);
            if (strs.empty() || (strs.size() == 1 && strs[0].empty()))
                return true;
            std::vector<const char*> argv;
            for (size_t i = 0; i < strs.size(); ++i)
            {
                argv.push_back(strs[i].c_str());
            }
            return handle_input((int)argv.size(), argv.data());
        }

        int handle_input(int argc, char* argv[])
        {
            std::vector<const char*> arg_vec;
            for (int i = 0; i < argc; ++i)
            {
                arg_vec.push_back(argv[i]);
            }
            return handle_input((int)arg_vec.size(), arg_vec.data());
        }

        int handle_input(int argc, const char* argv[])
        {
            if (argc <= 0 || cmders_.find(argv[0]) == cmders_.end())
            {
                std::cout << "invalid command, please retry input" << std::endl;
                return true;
            }
            auto result = cmders_[argv[0]]->handle_input(argc, argv);
            return result;
        }

        void show_help() const
        {
            std::string fmt_str = fmt::format("{}{}{}", "{:", cmder_max_length_ + 4, "}{}");
            for (auto& c : cmders_)
            {
                std::cout << fmt::format(fmt_str, c.first, c.second->desc()) << std::endl;
            }
        }
    protected:
        world_ptr_t world_ptr_;
        std::unordered_map<std::string, base_cmder_ptr> cmders_;
        int cmder_max_length_ = 0;
    };

    using icmder_manager_ptr = icmder_manager*;
}