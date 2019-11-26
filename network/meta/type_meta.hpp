#pragma once
#include "meta_common.hpp"
#include "network/base/type_map.hpp"
#include <string>

#define REG_TYPE(name) \
namespace ___reg_type_helper_value___ ## name ## __LINE__ { static int val = (cytx::type_meta_map::ins().set<name>(#name), 0); }

namespace cytx
{
    class type_meta_map : public base_type_map<std::string>
    {
    public:
        static type_meta_map& ins()
        {
            static type_meta_map t;
            return t;
        }
    };
}