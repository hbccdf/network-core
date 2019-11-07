#pragma once
#include <string>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <iostream>
#include <fmt/format.h>
#include <boost/program_options.hpp>
#include "network/meta/meta.hpp"

namespace cytx
{
    namespace bpo = boost::program_options;

    using bpo_option_t = bpo::option_description;
    using bpo_options_t = bpo::options_description;
    using bpo_pos_options_t = bpo::positional_options_description;

    namespace detail
    {
        using namespace boost::program_options;
        using namespace std;

        inline vector<string> get_args(string input)
        {
            return split_winmain(input);
        }

        inline bool all_default(const variables_map& vm)
        {
            return std::all_of(std::begin(vm), std::end(vm), [](const auto& it) -> bool {
                return it.second.defaulted();
            });
        }

        inline std::unique_ptr<options_description> options(string cmd_name)
        {
            auto str = cmd_name;
            str += " options";
            auto op = std::make_unique<options_description>(str);
            op->add_options()
                ("help,h", "show help info");
            return op;
        }

        inline variables_map get_vm(string input, options_description& op, positional_options_description& pd, bool show_help = true)
        {
            variables_map vm;
            auto pr = command_line_parser(get_args(input))
                .options(op)
                .positional(pd)
                .run();
            store(pr, vm);
            notify(vm);

            if (show_help && (vm.empty() || (vm.count("help") && vm.size() == 1) || all_default(vm)))
            {
                cout << op << endl;
                throw true;
            }
            return vm;
        }

        inline variables_map get_vm(int argc, const char* argv[], options_description& op, positional_options_description& pd, bool show_help = true)
        {
            variables_map vm;
            auto pr = command_line_parser(argc, argv)
                .options(op)
                .positional(pd)
                .run();
            store(pr, vm);

            if (show_help && (vm.empty() || (vm.count("help") && vm.size() == 1) || all_default(vm)))
            {
                cout << op << endl;
                throw true;
            }
            return vm;
        }
    }

    class icmder
    {
    public:
        virtual ~icmder() {}
    public:
        void init_options(std::string name, std::string desc)
        {
            name_ = name;
            desc_ = desc;
            op_ = detail::options(name);
            set_options();
        }

        virtual void set_options() {}

        virtual int handle_input(std::string input)
        {
            auto input_args = detail::get_args(input);
            std::vector<const char*> args;
            for (auto& s : input_args)
            {
                args.push_back(s.c_str());
            }
            return handle_input((int)args.size(), args.data());
        }

        virtual int handle_input(int argc, const char* argv[])
        {
            show_help();
            return 0;
        }

        void show_help() const
        {
            std::cout << (*op_) << std::endl;
        }

    public:
        std::string desc() { return desc_; }
        std::string name() { return name_; }
    protected:
        std::string name_;
        std::string desc_;
        std::unique_ptr<bpo_options_t> op_;
        bpo_pos_options_t pd_;
    };
}