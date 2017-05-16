#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <network/base/auto_mocker.h>

#include <network/orm/db_meta.hpp>

using namespace std;
using namespace cytx;
using namespace cytx::orm;

struct st
{
    int id;
    string name;
    short port;
    double val;
};
DB_META(st, id, name, port, val);

TEST(field_proxy, assign)
{
    st_query v;
    v.id = 3;
    assert_streq(v.id.str().c_str(), "st.id=3");

    v.id = v.id + 3;
    assert_streq(v.id.str().c_str(), "st.id=st.id+3");
}

TEST(field_proxy, operator)
{
    st_query v;
    v.id = v.id + 3;
    assert_streq(v.id.str().c_str(), "st.id=st.id+3");

    v.id = v.id - 3;
    assert_streq(v.id.str().c_str(), "st.id=st.id-3");

    v.id = v.id * 3;
    assert_streq(v.id.str().c_str(), "st.id=st.id*3");

    v.id = v.id / 3;
    assert_streq(v.id.str().c_str(), "st.id=st.id/3");

    v.id = v.id % 3;
    assert_streq(v.id.str().c_str(), "st.id=st.id%3");


    v.id += 3;
    assert_streq(v.id.str().c_str(), "st.id=st.id+3");

    v.id -= 3;
    assert_streq(v.id.str().c_str(), "st.id=st.id-3");

    v.id *= 3;
    assert_streq(v.id.str().c_str(), "st.id=st.id*3");

    v.id /= 3;
    assert_streq(v.id.str().c_str(), "st.id=st.id/3");

    v.id %= 3;
    assert_streq(v.id.str().c_str(), "st.id=st.id%3");
}