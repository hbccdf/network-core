#pragma once
#include "icmder.h"

namespace cytx
{
    class icmder_manager
    {
    public:
        void register_cmder(std::string cmd_name, icmder_ptr cmder)
        {
            cmders_[cmd_name] = cmder;
        }

        void register_cmder(icmder_ptr cmder)
        {
            register_cmder(cmder->name(), cmder);
        }

        bool handle_input(std::string input)
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

        bool handle_input(int argc, char* argv[])
        {
            std::vector<const char*> arg_vec;
            for (int i = 0; i < argc; ++i)
            {
                arg_vec.push_back(argv[i]);
            }
            return handle_input((int)arg_vec.size(), arg_vec.data());
        }

        bool handle_input(int argc, const char* argv[])
        {
            if (argc <= 0 || cmders_.find(argv[0]) == cmders_.end())
            {
                std::cout << "invalid command, please retry input" << std::endl;
                return true;
            }
            auto is_continue = cmders_[argv[0]]->handle_input(argc, argv);
            return is_continue;
        }

        void dump_help()
        {
            std::string fmt_str = "{:10}{}";
            for (auto& c : cmders_)
            {
                std::cout << fmt::format(fmt_str, c.first, c.second->desc()) << endl;
            }
        }
    protected:
        std::unordered_map<std::string, icmder_ptr> cmders_;
    };

    using icmder_manager_ptr = std::shared_ptr<icmder_manager>;
}