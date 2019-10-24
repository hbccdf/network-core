#pragma once
#include "network/service/base_service.hpp"
#include "cmder_service.h"
#include "cmder_reader.h"
#include <boost/algorithm/string.hpp>
#include <thread>

namespace cytx
{
    namespace detail
    {
        bool is_empty_or_white_spate(const std::string& str)
        {
            return str.empty() || boost::trim_copy(str).empty();
        }
    }

    struct cmd_node_info
    {
        int index;
        cmd_node* cmd;
    };

    class execute_cmder_service : public base_service
    {
    public:
        void init()
        {
            cmder_service_ = service_mgr_->get_service<cmder_service>();

            std::string cmder_config_path = world_ptr_->get_string_or("cmder_config_path", "config/cmd.xml");
            cmder_reader::read_config(config_, cmder_config_path);

            std::cout << "profile " << config_.profile << std::endl;
            cmds_ptr_ = &(config_.cmds[config_.profile]);

            for (int i = 0; (int)i < cmds_ptr_->size(); ++i)
            {
                cmd_node* cmd_node_ptr = &(cmds_ptr_->at(i));
                if (!cmd_node_ptr->name.empty())
                {
                    cmd_map_[cmd_node_ptr->name] = cmd_node_info{ i, cmd_node_ptr };
                }
            }
        }

    public:
        int execute()
        {
            int ret = 0;
            while (ret == 0 && cur_cmd_index >= 0 && cur_cmd_index < (int)cmds_ptr_->size())
            {
                ret = execute_one();
            }

            return ret;
        }

    private:
        int execute_one()
        {
            cmd_node* node = &(cmds_ptr_->at(cur_cmd_index));
            return execute_node(node);
        }

        int execute_node(cmd_node* node)
        {
            int ret = 0;
            if (!detail::is_empty_or_white_spate(node->cmdStr) && node->times > 0)
            {
                ret = repeat_execute_cmd_node(node);
            }

            return get_next_cmd_node_index(node, ret == 0);
        }

        int repeat_execute_cmd_node(cmd_node* node)
        {
            if (node->delay > 0)
            {
                sleep(node->delay);
            }

            int current_times = 0;
            while (current_times < node->times)
            {
                int ret = execute_cmd_string(node->cmdStr);
                if (ret != 0)
                {
                    return ret;
                }

                ++current_times;
                if (current_times < node->times && node->interval > 0)
                {
                    sleep(node->interval);
                }
            }

            return 0;
        }

        int execute_cmd_string(std::string cmd_str)
        {
            boost::trim(cmd_str);
            std::vector<std::string> strs;
            boost::split(strs, cmd_str, boost::is_any_of("\r\n"), boost::token_compress_on);

            for (auto& str : strs)
            {
                int ret = execute_cmd_one_line(str);
                if (ret != 0)
                {
                    return ret;
                }
            }

            return 0;
        }

        int execute_cmd_one_line(std::string cmd_str)
        {
            if (cmd_str.empty())
            {
                return 0;
            }

            boost::trim(cmd_str);
            //以注释开头
            if (boost::starts_with(cmd_str, "#"))
            {
                return 0;
            }

            return cmder_service_->handle_input(cmd_str);
        }

        int get_next_cmd_node_index(cmd_node* cur_cmd_ptr, bool is_success)
        {
            std::string next_cmd_name;
            next_cmd_name = is_success ? cur_cmd_ptr->nextCmd : cur_cmd_ptr->nextCmdWhenFailed;
            if (!next_cmd_name.empty())
            {
                auto it = cmd_map_.find(next_cmd_name);
                if (it != cmd_map_.end())
                {
                    return it->second.index;
                }
            }

            return ++cur_cmd_index;
        }

    private:
        void sleep(int milliseconds)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        }

    private:
        cmder_service* cmder_service_ = nullptr;
        cmder_config config_;
        std::vector<cmd_node>* cmds_ptr_ = nullptr;
        int cur_cmd_index = 0;

        std::unordered_map<std::string, cmd_node_info> cmd_map_;
    };
}