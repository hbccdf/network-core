#pragma once
#include "cmder_config.h"
#include "network/serialize/deserializer.hpp"
#include "network/serialize/xml_adapter.hpp"

namespace cytx
{
    class cmder_reader
    {
    public:
        static cmder_config get_config()
        {
            return get_config("cmd.xml");
        }

        static cmder_config get_config(std::string file)
        {
            cmder_config config;

            DeSerializer<xml_deserialize_adapter> de;
            de.enum_with_str(true);
            de.parse_file(file);

            de.DeSerialize(config, "config");
            return config;
        }

        static cmder_config get_config_from_string(std::string str)
        {
            cmder_config config;

            DeSerializer<xml_deserialize_adapter> de;
            de.enum_with_str(true);
            de.parse(str);

            de.DeSerialize(config, "config");
            return config;
        }
    };
}