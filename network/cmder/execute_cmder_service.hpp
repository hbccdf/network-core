#pragma once
#include "network/service/base_service.hpp"
#include "cmder_service.hpp"
#include "cmder_reader.hpp"
#include "network/base/log.hpp"
#include "network/util/thread.hpp"
#include "network/util/string.hpp"

namespace cytx
{
    struct cmd_node_info
    {
        int index;
        cmd_node* cmd;
    };

    class execute_cmder_service : public base_service
    {
    public:
        bool init()
        {
            cmder_service_ = service_mgr_->get();

            std::string cmder_config_path = world_ptr_->get_string_or("cmder_config_path", "config/cmd.xml");
            cmder_reader::read_config(config_, cmder_config_path);

            std::string profile = world_ptr_->get_string_or("profile", config_.profile);


            LOGGER_INFO("profile {}", profile);
            if (config_.cmds.find(profile) == config_.cmds.end())
            {
                LOGGER_ERROR("cannt find {} cmds, invalid profile", profile);
                return false;
            }

            cmder_service_->register_alias(config_.alias);

            cmds_ptr_ = &(config_.cmds[profile]);

            for (int i = 0; (int)i < cmds_ptr_->size(); ++i)
            {
                cmd_node* cmd_node_ptr = &(cmds_ptr_->at(i));
                if (!cmd_node_ptr->name.empty())
                {
                    cmd_map_[cmd_node_ptr->name] = cmd_node_info{ i, cmd_node_ptr };
                }
            }
            return true;
        }

    public:
        int execute()
        {
            int ret = 0;
            while (cur_cmd_index_ >= 0 && cur_cmd_index_ < (int)cmds_ptr_->size())
            {
                ret = execute_one();
            }

            return ret;
        }

    private:
        int execute_one()
        {
            cmd_node* node = &(cmds_ptr_->at(cur_cmd_index_));
            return execute_node(node);
        }

        int execute_node(cmd_node* node)
        {
            int ret = 0;
            if (!util::is_empty_or_white_spate(node->cmdStr) && node->times > 0)
            {
                ret = repeat_execute_cmd_node(node);
            }

            cur_cmd_index_ = get_next_cmd_node_index(node, ret == 0);
            return ret;
        }

        int repeat_execute_cmd_node(cmd_node* node)
        {
            if (node->delay > 0)
            {
                util::sleep(node->delay);
            }

            int current_times = 0;
            while (current_times < node->times)
            {
                int ret = execute_cmd_string(node->cmdStr);
                if (ret != 0)
                    return ret;

                ++current_times;
                if (current_times < node->times && node->interval > 0)
                {
                    util::sleep(node->interval);
                }
            }

            return 0;
        }

        int execute_cmd_string(std::string cmd_str)
        {
            string_util::trim(cmd_str);
            auto strs = string_util::split(cmd_str, "\r\n");

            for (auto& str : strs)
            {
                int ret = execute_cmd_one_line(str);
                if (ret != 0)
                    return ret;
            }

            return 0;
        }

        int execute_cmd_one_line(std::string cmd_str)
        {
            //去除结尾的注释
            string_util::trim_first(cmd_str, "#");

            string_util::trim(cmd_str);
            if (cmd_str.empty())
                return 0;

            //以注释开头
            if (boost::starts_with(cmd_str, "#"))
                return 0;

            return cmder_service_->handle_input(cmd_str);
        }

        int get_next_cmd_node_index(cmd_node* cur_cmd_ptr, bool is_success)
        {
            std::string next_cmd_name;
            next_cmd_name = is_success ? cur_cmd_ptr->nextCmd : cur_cmd_ptr->nextCmdWhenFailed;
            if (next_cmd_name.empty() && is_success)
            {
                return ++cur_cmd_index_;
            }
            else if (!next_cmd_name.empty())
            {
                auto it = cmd_map_.find(next_cmd_name);
                if (it != cmd_map_.end())
                {
                    return it->second.index;
                }
            }

            return -1;
        }

    private:
        cmder_service* cmder_service_ = nullptr;
        cmder_config config_;
        std::vector<cmd_node>* cmds_ptr_ = nullptr;
        int cur_cmd_index_ = 0;

        std::unordered_map<std::string, cmd_node_info> cmd_map_;
    };

    REG_SERVICE(execute_cmder_service);
}