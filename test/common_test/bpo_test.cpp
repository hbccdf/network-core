#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <network/base/auto_mocker.h>

#include <network/serialize.hpp>
#include <network/serialize/bpo_adapter.hpp>
using namespace cytx;
using namespace std;
using namespace boost::program_options;

template<typename T>
void assert_val_equal(T&& v, T&& dv)
{
    assert_eq(v, dv);
}
template<>
void assert_val_equal<float>(float&& v, float&& dv)
{
    assert_float_eq(v, dv);
}
template<>
void assert_val_equal<double>(double&& v, double&& dv)
{
    assert_double_eq(v, dv);
}

template<typename T, size_t N>
void assert_val_equal(T(&v)[N], T(&dv)[N])
{
    for (size_t i = 0; i < N; ++i)
    {
        assert_val_equal(v[i], dv[i]);
    }
}

template<size_t N>
void assert_val_equal(char(&v)[N], char(&dv)[N])
{
    assert_streq(v, dv);
}

class bpo_type : public ::testing::Test
{
public:
    DeSerializer<bpo_deserialize_adapter> de;
    options_description op;

    virtual void SetUp()
    {
    }

    virtual void TearDown() {}

    template<typename T>
    T get_de(const bpo::options_description& op, vector<string>& v)
    {
        using value_t = std::decay_t<T>;
        value_t dv;
        de.parse(v, op);
        de.DeSerialize(dv);
        return std::move(dv);
    }

    template<typename T>
    T get_de(const bpo::options_description& op, vector<string>& v, const char* key)
    {
        using value_t = std::decay_t<T>;
        value_t dv;
        de.parse(v, op);
        de.DeSerialize(dv, key);
        return std::move(dv);
    }

    template<typename T>
    auto get_de(const bpo::options_description& op, const char* cmd_line) -> std::enable_if_t<!is_user_class_v<T>, T>
    {
        using value_t = std::decay_t<T>;
        value_t dv;
        de.parse(cmd_line, op);
        de.DeSerialize(dv);
        return std::move(dv);
    }

    template<typename T>
    auto get_de(bpo::options_description& op, const char* cmd_line) -> std::enable_if_t<is_user_class_v<T>, T>
    {
        using value_t = std::decay_t<T>;
        value_t dv;
        de.init(dv, op);
        de.parse(cmd_line);
        de.DeSerialize(dv);
        return std::move(dv);
    }

    template<typename T>
    T get_de(const bpo::options_description& op, const char* key, const char* cmd_line)
    {
        using value_t = std::decay_t<T>;
        value_t dv;
        de.parse(cmd_line, op);
        de.DeSerialize(dv, key);
        return std::move(dv);
    }
};

TEST_F(bpo_type, bool)
{
    op.add_options()
        ("help,h", "show help info");
    auto dv = get_de<bool>(op, "help", "-h");
    assert_eq(dv, true);
}

TEST_F(bpo_type, no_bool)
{
    op.add_options()
        ("help,h", "show help info");
    auto dv = get_de<bool>(op, "help", "");
    assert_eq(dv, false);
}

TEST_F(bpo_type, int_throw)
{
    op.add_options()
        ("test", "show help info");
    assert_throw(get_de<int>(op, "test", "--test"), boost::bad_any_cast);
}

TEST_F(bpo_type, int_no_throw)
{
    op.add_options()
        ("test", cytx::value<int>(), "show help info");
    auto dv = get_de<int>(op, "test", "--test 3");
    assert_eq(dv, 3);
}

enum class test1
{
    first,
    second,
    thire,
};
REG_ENUM(test1, first, second, thire);

TEST_F(bpo_type, enum)
{
    op.add_options()
        ("test", cytx::value<int>(), "show help info");
    auto dv = get_de<test1>(op, "test", "--test 2");
    assert_eq(dv, test1::thire);
}

TEST_F(bpo_type, enum_str)
{
    op.add_options()
        ("test", cytx::value<std::string>(), "show help info");
    de.enum_with_str(true);
    auto dv = get_de<test1>(op, "test", "--test thire");
    assert_eq(dv, test1::thire);
}

TEST_F(bpo_type, vector)
{
    op.add_options()
        ("test", cytx::value<vector<int>>(), "show help info");
    de.enum_with_str(true);
    auto dv = get_de<vector<int>>(op, "test", "--test=1 --test=3");
    assert_eq(dv[0], 1);
    assert_eq(dv[1], 3);
}

TEST_F(bpo_type, struct)
{
    struct st
    {
        bool help;
        META(help);
    };
    op.add_options()
        ("help,h", "show help info");
    de.enum_with_str(true);
    auto dv = get_de<st>(op, "-h");
    assert_eq(dv.help, true);
}

TEST_F(bpo_type, struct_int)
{
    struct st
    {
        int test;
        META(test);
    };
    op.add_options()
        ("test,t", cytx::value<int>(), "show help info");
    de.enum_with_str(true);
    auto dv = get_de<st>(op, "-t 10");
    auto is_empty = de.empty();
    assert_eq(is_empty, false);
    assert_eq(dv.test, 10);
}

TEST_F(bpo_type, struct_int1)
{
    struct st
    {
        int test;
        META(test);
    };
    op.add_options()
        ("test,t", cytx::value<int>(), "show help info");
    de.enum_with_str(true);
    auto dv = get_de<st>(op, "");
    auto is_empty = de.empty();
    assert_eq(is_empty, true);
}

TEST_F(bpo_type, struct_date_time)
{
    struct st
    {
        date_time dt;
        META(dt);
    };

    op.add_options()
        ("dt", "dt");
    de.enum_with_str(true);

    auto dv = get_de<st>(op, R"(--dt "2019-11-13 17:02:03")");
    date_time dd("2019-11-13 17:02:03");

    assert_eq(dv.dt, dd);
}