#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <network/base/auto_mocker.h>

#include <network/serialize.hpp>


using namespace cytx;

TEST(tuple_total_size, base_type)
{
    assert_eq(tuple_total_size<bool>::value, 1);
    assert_eq(tuple_total_size<char>::value, 1);
    assert_eq(tuple_total_size<unsigned char>::value, 1);
    assert_eq(tuple_total_size<short>::value, 2);
    assert_eq(tuple_total_size<unsigned short>::value, 2);
    assert_eq(tuple_total_size<int>::value, 4);
    assert_eq(tuple_total_size<unsigned int>::value, 4);
    assert_eq(tuple_total_size<int64_t>::value, 8);
    assert_eq(tuple_total_size<uint64_t>::value, 8);
    assert_eq(tuple_total_size<float>::value, 4);
    assert_eq(tuple_total_size<double>::value, 8);
}

TEST(tuple_total_size, base_type_array)
{
    assert_eq(tuple_total_size<char[4]>::value, 4);
    assert_eq(tuple_total_size<float[3]>::value, 12);
    assert_eq((tuple_total_size<std::array<char, 3>>::value), 3);
    assert_eq((tuple_total_size<std::array<float, 10>>::value), 40);
}

enum old_char : char
{
    oca,
};

enum old_short : short
{
    ocs,
};

enum old_int : int
{
    oi
};

enum old_enum
{
    oe
};

enum class new_char : char
{
    ncc
};

enum class new_enum
{
    nce
};

TEST(tuple_total_size, enum)
{
    assert_eq(tuple_total_size<old_char>::value, 1);
    assert_eq(tuple_total_size<old_short>::value, 2);
    assert_eq(tuple_total_size<old_int>::value, 4);
    assert_eq(tuple_total_size<old_enum>::value, 4);
    assert_eq(tuple_total_size<old_enum[4]>::value, 16);
}

TEST(tuple_total_size, tuple)
{
    assert_eq(tuple_total_size<std::tuple<>>::value, 0);
    assert_eq(tuple_total_size<std::tuple<int>>::value, 4);
    assert_eq(tuple_total_size<std::tuple<char>>::value, 1);
    assert_eq((tuple_total_size<std::tuple<char, int>>::value), 5);
    assert_eq((tuple_total_size<std::pair<int, int>>::value), 8);
}

TEST(tuple_total_size, invalid_type)
{
    assert_eq(tuple_total_size<std::string>::value, -1);
    assert_eq(tuple_total_size<std::tuple<std::string>>::value, -1);
    assert_eq((tuple_total_size<std::tuple<int, std::string>>::value), -1);
    assert_eq(tuple_total_size<std::vector<int>>::value, -1);
    assert_eq(tuple_total_size<std::list<int>>::value, -1);
    assert_eq((tuple_total_size<std::array<std::string, 3>>::value), -1);
    assert_eq((tuple_total_size<std::pair<int, std::string>>::value), -1);
}

struct st_base
{
    int v;
    META(v);
};

struct st_base1
{
    int vi;
    float vf;
    double vd;
    META(vi, vf, vd);
};

struct st_base_and_base1
{
    st_base base;
    st_base1 base1;
    META(base, base1);
};

struct st_invalid
{
    int v;
    std::string str;
    META(v, str);
};

TEST(tuple_total_size, struct)
{
    assert_eq(tuple_total_size<st_base>::value, 4);
    assert_eq(tuple_total_size<st_base1>::value, 16);
    assert_eq(tuple_total_size<st_base_and_base1>::value, 20);
    assert_eq(tuple_total_size<st_invalid>::value, -1);
}