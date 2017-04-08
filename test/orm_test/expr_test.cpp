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
    DB_META(st, id, name, port, val);
};

TEST(field_proxy, assign)
{
    st v;
    v._id = 3;
    assert_streq(v._id.str().c_str(), "st.id=3");

    v._id = v._id + 3;
    assert_streq(v._id.str().c_str(), "st.id=st.id+3");
}

TEST(field_proxy, operator)
{
    st v;
    v._id = v._id + 3;
    assert_streq(v._id.str().c_str(), "st.id=st.id+3");

    v._id = v._id - 3;
    assert_streq(v._id.str().c_str(), "st.id=st.id-3");

    v._id = v._id * 3;
    assert_streq(v._id.str().c_str(), "st.id=st.id*3");

    v._id = v._id / 3;
    assert_streq(v._id.str().c_str(), "st.id=st.id/3");

    v._id = v._id % 3;
    assert_streq(v._id.str().c_str(), "st.id=st.id%3");


    v._id += 3;
    assert_streq(v._id.str().c_str(), "st.id=st.id+3");

    v._id -= 3;
    assert_streq(v._id.str().c_str(), "st.id=st.id-3");

    v._id *= 3;
    assert_streq(v._id.str().c_str(), "st.id=st.id*3");

    v._id /= 3;
    assert_streq(v._id.str().c_str(), "st.id=st.id/3");

    v._id %= 3;
    assert_streq(v._id.str().c_str(), "st.id=st.id%3");
}