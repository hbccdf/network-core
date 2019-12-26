#pragma once
#include "cmder_config.hpp"
#include "network/serialize/deserializer.hpp"
#include "network/serialize/xml_adapter.hpp"

namespace cytx
{
    class cmder_reader
    {
    public:
        static cmder_config get_config()
        {
            cmder_config config;
            read_config(config);
            return config;
        }

        static cmder_config get_config(std::string file)
        {
            cmder_config config;
            read_config(config, file);
            return config;
        }

        static cmder_config get_config_from_string(std::string str)
        {
            cmder_config config;
            read_config(config, str);
            return config;
        }

        static void read_config(cmder_config& config)
        {
            read_config(config, "cmd.xml");
        }

        static void read_config(cmder_config& config, std::string file)
        {
            DeSerializer<xml_deserialize_adapter> de;
            de.enum_with_str(true);
            de.parse_file(file, true);

            de.DeSerialize(config, "config");
        }

        static void read_config_from_string(cmder_config& config, std::string str)
        {
            DeSerializer<xml_deserialize_adapter> de;
            de.enum_with_str(true);
            de.parse(str);

            de.DeSerialize(config, "config");
        }
    };
}