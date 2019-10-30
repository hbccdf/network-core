#pragma once
#include "whitespace_config.hpp"

namespace cytx
{
    struct xml_parser
    {
        template<typename... Args>
        static void write(Args&&... args)
        {
            using namespace boost::property_tree;
            auto setting = xml_writer_make_settings<std::string>(' ', 4);
            write_xml(std::forward<Args>(args)..., setting);
        }

        template<typename... Args>
        static void read(Args&&... args)
        {
            using namespace boost::property_tree;
            using namespace boost::property_tree::xml_parser;
            read_xml(std::forward<Args>(args)..., trim_whitespace | no_comments);
        }

        template<typename... Args>
        static void read_file(Args&&... args)
        {
            using namespace boost::property_tree;
            using namespace boost::property_tree::xml_parser;
            read_xml(std::forward<Args>(args)..., trim_whitespace | no_comments, std::locale());
        }
    };

    struct info_parser
    {
        template<typename... Args>
        static void write(Args&&... args)
        {
            using namespace boost::property_tree;
            write_info(std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void read(Args&&... args)
        {
            using namespace boost::property_tree;
            read_info(std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void read_file(Args&&... args)
        {
            using namespace boost::property_tree;
            read_info(std::forward<Args>(args)..., std::locale());
        }
    };

    struct ini_parser
    {
        template<typename... Args>
        static void write(Args&&... args)
        {
            using namespace boost::property_tree;
            write_ini(std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void read(Args&&... args)
        {
            using namespace boost::property_tree;
            read_ini(std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void read_file(Args&&... args)
        {
            using namespace boost::property_tree;
            read_ini(std::forward<Args>(args)..., std::locale());
        }
    };
}