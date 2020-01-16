#pragma once
#include "base_cmder.hpp"
#include "network/base/world.hpp"
#include "network/util/string.hpp"
#include <mutex>

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

        void register_cmder(const std::string& cmd_name, base_cmder_ptr cmder)
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

        void register_alias(const std::unordered_map<std::string, std::string>& alias)
        {
            alias_ = alias;
        }

        base_cmder_ptr get_cmder(const std::string& cmd_name) const
        {
            auto it = cmders_.find(cmd_name);
            if (it != cmders_.end())
            {
                return it->second;
            }

            return nullptr;
        }

        int handle_input(const std::string& input)
        {
            auto strs = detail::get_args(input);
            if (strs.empty() || (strs.size() == 1 && strs[0].empty()))
                return true;

            auto new_args = process_alias(strs);
            return internal_handle_input(new_args);
        }

        int handle_input(int argc, char* argv[])
        {
            std::vector<std::string> args;
            for (int i = 0; i < argc; ++i)
            {
                args.push_back(argv[i]);
            }
            auto new_args = process_alias(args);
            return internal_handle_input(new_args);
        }

        int handle_input(int argc, const char* argv[])
        {
            std::vector<std::string> args;
            for (int i = 0; i < argc; ++i)
            {
                args.push_back(argv[i]);
            }
            auto new_args = process_alias(args);
            return internal_handle_input(new_args);
        }

        void show_help() const
        {
            std::string fmt_str = fmt::format("{}{}{}", "{:", cmder_max_length_ + 4, "}{}");
            for (auto& c : cmders_)
            {
                std::cout << fmt::format(fmt_str, c.first, c.second->desc()) << std::endl;
            }
        }

        const std::unordered_map<std::string, std::string>& get_alias() const
        {
            return alias_;
        }

    public:
        void set_waiting_cmder(base_cmder_ptr cmder)
        {
            std::unique_lock<std::mutex> locker(mutex_);
            waiting_cmder_ = cmder;
        }

        void clear_waiting_cmder()
        {
            std::unique_lock<std::mutex> locker(mutex_);
            waiting_cmder_ = nullptr;
        }

        void notify_waiting_cmder(int result)
        {
            base_cmder_ptr tmp_waiting_cmder = nullptr;
            {
                std::unique_lock<std::mutex> locker(mutex_);
                tmp_waiting_cmder = waiting_cmder_;
                waiting_cmder_ = nullptr;
            }

            if (tmp_waiting_cmder != nullptr)
            {
                tmp_waiting_cmder->notify_waiting_result(result);
            }
        }

    private:
        int internal_handle_input(std::vector<std::string>& args)
        {
            if (args.empty())
            {
                std::cout << "invalid command, please retry input" << std::endl;
                return 1;
            }

            std::string cmd_name = args[0];
            if (cmders_.find(cmd_name) == cmders_.end())
            {
                std::cout << fmt::format("not find cmder {}, please retry input") << std::endl;
                return 1;
            }

            args.erase(args.begin());

            return cmders_[cmd_name]->handle_input(args);
        }

        std::vector<std::string> process_alias(std::vector<std::string>& args)
        {
            std::vector<std::string> list;
            if (args.empty())
                return list;

            auto it = alias_.find(args[0]);
            if (it == alias_.end())
            {
                list = args;
                return list;
            }

            //finded alias
            auto alias_str = it->second;
            //移除注释
            string_util::trim_first(alias_str, "#");
            auto alias_args = detail::get_args(alias_str);
            for (auto& arg : alias_args)
            {
                list.emplace_back(arg);
            }

            for (int i = 1; i < (int)args.size(); ++i)
            {
                list.push_back(args[i]);
            }

            return list;
        }

    protected:
        world_ptr_t world_ptr_;
        std::unordered_map<std::string, base_cmder_ptr> cmders_;
        int cmder_max_length_ = 0;

        std::mutex mutex_;
        base_cmder_ptr waiting_cmder_;

        std::unordered_map<std::string, std::string> alias_;
    };

    using icmder_manager_ptr = icmder_manager*;
}