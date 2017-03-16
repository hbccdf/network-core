#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "auto_mocker.h"

#include "network/serialize/common.hpp"
#include "network/serialize/enum.hpp"
#include "network/serialize/serializer.hpp"
#include "network/serialize/deserializer.hpp"

using namespace cytx;

enum class t : int
{
    a = 9,
    b,
    c,
};

REG_ENUM(t, a, b, c);

using namespace std;

TEST(enum, b)
{
    auto str = to_string(t::b);
    assert_strcaseeq(str.value().c_str(), "t::b");
    assert_true(!to_string((t)1));
}

TEST(enum, int)
{
    auto v = to_enum<t>("b");
    assert_eq(v.value(), t::b);
}

enum class test_mode
{
    init,
    start,
    monitor,
    remote,
};

REG_ENUM(test_mode, init, start, monitor, remote);

TEST(enum, spe)
{
    Serializer<json_serialize_adapter> se;
    se.enum_with_str(true);

    struct p
    {
        test_mode mode;
        int i;
        META(mode, i);
    };

    p v{ test_mode::remote, 1 };
    se.Serialize(v);
    auto str = se.get_adapter().str();
    
    DeSerializer<json_deserialize_adapter> de;
    de.enum_with_str(true);

    p dv;
    de.parse(str);
    de.DeSerialize(dv);

    assert_eq(v.mode, dv.mode);
    assert_eq(v.i, dv.i);
}