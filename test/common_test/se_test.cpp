#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <network/base/auto_mocker.h>

#include <network/serialize.hpp>
using namespace cytx;
using namespace std;

class base_type : public ::testing::Test
{
public:
    Serializer<json_serialize_adapter> se;
    DeSerializer<json_deserialize_adapter> de;

    virtual void SetUp() {

    }

    virtual void TearDown() {}

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

    /*template<typename T>
    auto assert_val_equal(T&& v, T&& dv) -> std::enable_if_t<is_container<T>::value>
    {
        auto itv = v.begin();
        auto it_dv = dv.begin();
        for (; it != v.end(); ++it, ++it_dv)
        {
            assert_val_equal(*it, *it_dv);
        }
    }*/

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

    template<typename T>
    void assert_equal(T&& v)
    {
        using value_t = std::decay_t<T>;
        value_t dv;
        se.Serialize(v);
        auto str = se.get_adapter().str();
        de.parse(str);
        de.DeSerialize(dv);
        assert_val_equal(v, dv);
    }

    template<typename T, size_t N>
    void assert_equal(T(&v)[N])
    {
        using value_t = std::decay_t<T>;
        value_t dv[N];
        se.Serialize(v);
        auto str = se.get_adapter().str();
        de.parse(str);
        de.DeSerialize(dv);
        assert_val_equal(v, dv);
    }

    template<typename T>
    T get_de(T& v)
    {
        using value_t = std::decay_t<T>;
        value_t dv;
        se.Serialize(v);
        auto str = se.get_adapter().str();
        de.parse(str);
        de.DeSerialize(dv);
        return std::move(dv);
    }

    template<typename T>
    T get_de(string str)
    {
        using value_t = std::decay_t<T>;
        value_t dv;
        de.parse(str);
        de.DeSerialize(dv);
        return std::move(dv);
    }
};

TEST(tuple_contains, test)
{
    using tt = std::tuple<int, std::string>;
    tt t{ 1, "hello" };
    bool val = tuple_contains<int, tt>::value;
    bool val1 = tuple_contains<float, tt>::value;
    bool val2 = tuple_contains<float, std::tuple<float>>::value;
    bool val3 = tuple_contains<float, std::tuple<>>::value;
    assert_eq(val, true);
    assert_eq(val1, false);
    assert_eq(val2, true);
    assert_eq(val3, false);
}

TEST(tuple_contains, de)
{
    std::tuple<string, int, float> v{ "hello", 1, 1.9f };
    DeSerializer<json_deserialize_adapter, std::tuple<string, float>> de;
    de.set_tuple({ "hello", 1.9f });
    de.parse("[1]");
    std::tuple<string, int, float> dv;
    de.DeSerialize(dv);
    assert_eq(v, dv);
}

TEST_F(base_type, bool)
{
    assert_equal(true);
}

TEST_F(base_type, char)
{
    assert_equal('t');
}

TEST_F(base_type, unsigned_char)
{
    unsigned char v = 'A';
    assert_equal(v);
}

TEST_F(base_type, short)
{
    short v = 300;
    assert_equal(v);
}

TEST_F(base_type, ushort)
{
    unsigned short v = 4000;
    assert_equal(v);
}

TEST_F(base_type, int)
{
    int v = 159 * 1024;
    assert_equal(v);
}

TEST_F(base_type, uint)
{
    unsigned int v = 259 * 1024;
    assert_equal(v);
}

TEST_F(base_type, float)
{
    float v = 9.9f;
    assert_equal(v);
}

TEST_F(base_type, double)
{
    double v = 3125.9534623445;
    assert_equal(v);
}

TEST_F(base_type, string)
{
    string v = "asdfefasdf";
    assert_equal(v);

    v = "";
    assert_equal(v);
}

TEST_F(base_type, array_int)
{
    int v[2]{ 3, 9 };
    assert_equal(v);
}

TEST_F(base_type, array_char)
{
    char v[10] = { "hello" };
    assert_equal(v);
}

TEST_F(base_type, array_bool)
{
    bool v[4]{ true, false, false, true };
    assert_equal(v);
}

TEST_F(base_type, array_float)
{
    float v[4]{ 0.2345f, -9.981f, 3361.234f, 1e2f };
    assert_equal(v);
}

TEST_F(base_type, array_double)
{
    double v[4]{ 0.2345, -9.981, 3361.234, 1e2 };
    assert_equal(v);
}

TEST_F(base_type, std_array)
{
    std::array<int, 3> v{ 5, -1234, 3 };
    assert_equal(v);
}

TEST_F(base_type, tuple)
{
    std::tuple<int, int> v{ 5, -1234};
    assert_equal(v);
}

TEST_F(base_type, tuple_and_tuple)
{
    std::tuple<int, int, std::tuple<string, double, bool>> v{ 5, -1234, {"hello", 3.158, true} };
    assert_equal(v);
}

TEST_F(base_type, pair)
{
    std::pair<int, int> v{ 5, -1234 };
    assert_equal(v);
}

TEST_F(base_type, pair_string)
{
    std::pair<string, int> v{ "this is for test", -1234 };
    assert_equal(v);
}

TEST_F(base_type, enum)
{
    enum en { val = 1, val2 = 4 };
    en v = val2;
    assert_equal(v);
}

TEST_F(base_type, enum_class)
{
    enum class en { val = 1, val2 = 4 };
    en v = en::val2;
    assert_equal(v);
}

TEST_F(base_type, enum_class_short)
{
    enum class en : short  { val = 1, val2 = 4 };
    en v = en::val2;
    assert_equal(v);
}

TEST_F(base_type, vector)
{
    std::vector<int> v = { 3, 9 };
    assert_equal(v);
}

TEST_F(base_type, list)
{
    std::list<int> v = { 3, 9 };
    assert_equal(v);
}

TEST_F(base_type, set)
{
    std::set<int> v = { 3, 9 };
    assert_equal(v);
}

TEST_F(base_type, unordered_set)
{
    std::unordered_set<int> v = { 3, 9 };
    assert_equal(v);
}

TEST_F(base_type, map)
{
    std::map<int, int> v = { { 3, 9 },{ 2,4 } };
    assert_equal(v);
}

TEST_F(base_type, unordered_map)
{
    std::unordered_map<int, int> v = { { 3, 9 },{ 2,4 } };
    assert_equal(v);
}

TEST_F(base_type, struct_bool)
{
    struct st
    {
        bool val;
        META(val);
    };
    st v = { true };
    auto dv = get_de(v);
    assert_val_equal(v.val, dv.val);
}

TEST_F(base_type, struct_more_field)
{
    struct st
    {
        bool val;
        int vi;
        float vf;
        string vs;
        char vcs[16];
        META(val, vi, vf, vs, vcs);
    };
    st v = { true,  13, 2.2f, "this is a test for meta", "hello world" };
    auto dv = get_de(v);
    assert_val_equal(v.val, dv.val);
    assert_val_equal(v.vi, dv.vi);
    assert_val_equal(v.vf, dv.vf);
    assert_val_equal(v.vs, dv.vs);
    assert_val_equal(v.vcs, dv.vcs);
}

TEST_F(base_type, struct_with_vector)
{
    struct st
    {
        bool val;
        int vi;
        float vf;
        string vs;
        char vcs[16];
        vector<int> vv;
        META(val, vi, vf, vs, vcs, vv);
    };
    st v = { true,  13, 2.2f, "this is a test for meta", "hello world",{ 3, 9, 100 } };
    auto dv = get_de(v);
    assert_val_equal(v.val, dv.val);
    assert_val_equal(v.vi, dv.vi);
    assert_val_equal(v.vf, dv.vf);
    assert_val_equal(v.vs, dv.vs);
    assert_val_equal(v.vcs, dv.vcs);
    assert_val_equal(v.vv, dv.vv);
}

TEST_F(base_type, struct_with_list)
{
    struct st
    {
        bool val;
        int vi;
        float vf;
        string vs;
        char vcs[16];
        list<string> vls;
        META(val, vi, vf, vs, vcs, vls);
    };
    st v = { true,  13, 2.2f, "this is a test for meta", "hello world",{ "name", "age", "test" } };
    auto dv = get_de(v);
    assert_val_equal(v.val, dv.val);
    assert_val_equal(v.vi, dv.vi);
    assert_val_equal(v.vf, dv.vf);
    assert_val_equal(v.vs, dv.vs);
    assert_val_equal(v.vcs, dv.vcs);
    assert_val_equal(v.vls, dv.vls);
}

TEST_F(base_type, struct_with_map)
{
    struct st
    {
        bool val;
        int vi;
        map<int, string> vm;
        float vf;
        string vs;
        char vcs[16];
        META(val, vi, vm, vf, vs, vcs);
    };
    st v = { true,  13,{ { 2, "name" },{ 9, "test" },{ -1, "age" } }, 2.2f, "this is a test for meta", "hello world" };
    auto dv = get_de(v);
    assert_val_equal(v.val, dv.val);
    assert_val_equal(v.vi, dv.vi);
    assert_val_equal(v.vf, dv.vf);
    assert_val_equal(v.vs, dv.vs);
    assert_val_equal(v.vcs, dv.vcs);
    assert_val_equal(v.vm, dv.vm);
}

TEST_F(base_type, struct_with_struct)
{
    struct person
    {
        int age;
        string name;
        META(age, name);
    };
    struct st
    {
        bool val;
        int vi;
        map<int, string> vm;
        float vf;
        string vs;
        char vcs[16];
        person vp;
        META(val, vi, vm, vf, vs, vcs, vp);
    };
    st v = { true,  13,{ { 2, "name" },{ 9, "test" },{ -1, "age" } }, 2.2f, "this is a test for meta", "hello world",{ 3, "demo" } };
    auto dv = get_de(v);
    assert_val_equal(v.val, dv.val);
    assert_val_equal(v.vi, dv.vi);
    assert_val_equal(v.vf, dv.vf);
    assert_val_equal(v.vs, dv.vs);
    assert_val_equal(v.vcs, dv.vcs);
    assert_val_equal(v.vm, dv.vm);
    assert_val_equal(v.vp.age, dv.vp.age);
    assert_val_equal(v.vp.name, dv.vp.name);
}

TEST_F(base_type, struct_with_struct_with_struct)
{
    struct test
    {
        float f;
        int c;
        META(f, c);
    };
    struct person
    {
        int age;
        string name;
        test t;
        META(age, name, t);
    };
    struct st
    {
        bool val;
        int vi;
        map<int, string> vm;
        float vf;
        string vs;
        char vcs[16];
        person vp;
        META(val, vi, vm, vf, vs, vcs, vp);
    };
    st v = { true,  13,{ { 2, "name" },{ 9, "test" },{ -1, "age" } }, 2.2f, "this is a test for meta", "hello world",{ 3, "demo",{ -100.235f, 10 } } };
    auto dv = get_de(v);
    assert_val_equal(v.val, dv.val);
    assert_val_equal(v.vi, dv.vi);
    assert_val_equal(v.vf, dv.vf);
    assert_val_equal(v.vs, dv.vs);
    assert_val_equal(v.vcs, dv.vcs);
    assert_val_equal(v.vm, dv.vm);
    assert_val_equal(v.vp.age, dv.vp.age);
    assert_val_equal(v.vp.name, dv.vp.name);
    assert_val_equal(v.vp.t.f, dv.vp.t.f);
    assert_val_equal(v.vp.t.c, dv.vp.t.c);
}

TEST_F(base_type, enum_class_short_with_struct)
{
    enum class en : short { val = 1, val2 = 4 };
    struct st
    {
        int vi;
        en ve;
        META(vi, ve);
    };

    st v = { 3, en::val };
    auto dv = get_de(v);
    assert_val_equal(v.vi, dv.vi);
    assert_val_equal(v.ve, dv.ve);
}

TEST_F(base_type, pair_with_pair)
{
    std::pair<int, std::pair<string, int>> v{ 5, {"world", -1234 } };
    assert_equal(v);
}

TEST_F(base_type, tuple_with_tuple)
{
    std::tuple<int, std::tuple<string, int>> v{ 5,{ "world", -1234 } };
    assert_equal(v);
}

TEST_F(base_type, tuple_with_tuple1)
{
    std::tuple<int, std::tuple<string, int>, std::tuple<int, string, bool, float>> v{ 5,{ "world", -1234 }, {4, "hello", true, 3.1415926f} };
    assert_equal(v);
}

TEST_F(base_type, pair_with_tuple)
{
    std::pair<int, std::tuple<std::tuple<string, int>, std::tuple<int, string, bool, float>, std::pair<int, int>>> v{  5,{ { "world", -1234 },{ 4, "hello", true, 3.1415926f } , {3, 3} } };
    assert_equal(v);
}

TEST_F(base_type, tuple_with_pair)
{
    std::tuple<int, std::pair<string, int>> v{ 5,{ "world", -1234 } };
    assert_equal(v);
}

TEST_F(base_type, tuple_with_pair_with_tuple)
{
    std::tuple<int, std::pair<string, std::tuple<int, string, int>>> v{ 5,{ "world", {3, "hello", -1234 } } };
    assert_equal(v);
}

TEST_F(base_type, struct_with_pair)
{
    struct st
    {
        std::pair<int, string> p;
        META(p);
    };
    st v{ {3, "test"} };
    auto dv = get_de(v);
    assert_val_equal(v.p, dv.p);
}

TEST_F(base_type, struct_with_tuple)
{
    struct st
    {
        std::tuple<int, string, int> p;
        META(p);
    };
    st v{ { 3, "test", -19 } };
    auto dv = get_de(v);
    assert_val_equal(v.p, dv.p);
}

TEST_F(base_type, struct_with_pair_with_tuple)
{
    struct st
    {
        std::pair<string, std::tuple<int, string, int>> p;
        META(p);
    };
    st v{ {"just test",{ 3, "test", -19 } } };
    auto dv = get_de(v);
    assert_val_equal(v.p, dv.p);
}

TEST_F(base_type, struct_with_tuple_with_pair)
{
    struct st
    {
        std::tuple<string, int, std::tuple<string, int>> p;
        META(p);
    };
    st v{ { "just test",3, { "test", -19 } } };
    auto dv = get_de(v);
    assert_val_equal(v.p, dv.p);
}

TEST_F(base_type, struct_with_tuple_and_pair)
{
    struct st
    {
        std::tuple<string, int, std::tuple<string, int>> t;
        std::pair<float, string> p;
        META(t, p);
    };
    st v{ { "just test",3,{ "test", -19 } }, {-3.14f, "helow"} };
    auto dv = get_de(v);
    assert_val_equal(v.p, dv.p);
    assert_val_equal(v.t, dv.t);
}

TEST_F(base_type, struct_with_reverse_order)
{
    struct st
    {
        std::tuple<string, int, std::tuple<string, int>> t;
        std::pair<float, string> p;
        int i;
        META(i, p, t);
    };
    st v{ { "just test",3,{ "test", -19 } },{ -3.14f, "helow" }, 9 };
    auto dv = get_de(v);
    assert_val_equal(v.p, dv.p);
    assert_val_equal(v.t, dv.t);
    assert_val_equal(v.i, dv.i);
}

TEST_F(base_type, struct_with_json)
{
    struct st
    {
        int i;
        float f;
        META(i, f);
    };
    st v{ 3, 2.2f };
    auto dv = get_de<st>(R"({"i":3, "f":2.2})");
    assert_val_equal(v.i, dv.i);
    assert_val_equal(v.f, dv.f);
}

TEST_F(base_type, struct_with_not_all_json)
{
    struct st
    {
        int i;
        float f;
        string str;
        META(i, f, str);
    };
    st v{ 1, 2.2f, "hello" };
    auto dv = get_de<st>(R"({"str":"hello","f":2.2})");
    assert_val_equal(v.f, dv.f);
    assert_val_equal(v.str, dv.str);
}

TEST_F(base_type, not_all_json_with_not_sort_order)
{
    struct st
    {
        int i;
        std::pair<int, int> p;
        META(i, p);
    };
    st v{ 1, {2, 3} };
    auto dv = get_de<st>(R"({"p":{"2":3}, "i":1})");
    assert_val_equal(v.i, dv.i);
    assert_val_equal(v.p, dv.p);
}

TEST_F(base_type, not_all_json_with_pair)
{
    struct st
    {
        int i;
        std::pair<int, int> p;
        META(i, p);
    };
    st v{ 1,{ 2, 3 } };
    auto dv = get_de<st>(R"({"p":{"2":3}})");
    assert_val_equal(v.p, dv.p);
}

TEST_F(base_type, not_all_json_with_tuple)
{
    struct st
    {
        int i;
        std::tuple<int, int> p;
        META(i, p);
    };
    st v{ 1,{ 2, 3 } };
    auto dv = get_de<st>(R"({"p":[2, 3]})");
    assert_val_equal(v.p, dv.p);
}

TEST_F(base_type, not_all_json_with_struct)
{
    struct person
    {
        int i;
        float f;
        META(i, f);
    };
    struct st
    {
        int i;
        std::tuple<int, int> p;
        person pe;
        META(i, p, pe);
    };
    st v{ 1,{ 2, 3 }, {3, 3.9f} };
    auto dv = get_de<st>(R"({"i":1, "pe":{"f":3.9}})");
    assert_val_equal(v.i, dv.i);
    assert_val_equal(v.pe.f, dv.pe.f);
}

TEST_F(base_type, struct_with_pair_with_specal_char)
{
    struct st
    {
        std::pair<string, int> p;
        META(p);
    };
    {
        st v{ { "\"", 3 } };
        auto dv = get_de(v);
        assert_val_equal(v.p, dv.p);
    }
    {
        st v{ { "@", 3 } };
        auto dv = get_de(v);
        assert_val_equal(v.p, dv.p);
    }
    {
        st v{ { "'", 3 } };
        auto dv = get_de(v);
        assert_val_equal(v.p, dv.p);
    }
    {
        st v{ { "\"\"", 3 } };
        auto dv = get_de(v);
        assert_val_equal(v.p, dv.p);
    }
    {
        st v{ { ";", 3 } };
        auto dv = get_de(v);
        assert_val_equal(v.p, dv.p);
    }
    {
        st v{ { ".", 3 } };
        auto dv = get_de(v);
        assert_val_equal(v.p, dv.p);
    }
    {
        st v{ { "-", 3 } };
        auto dv = get_de(v);
        assert_val_equal(v.p, dv.p);
    }
    {
        st v{ { "/", 3 } };
        auto dv = get_de(v);
        assert_val_equal(v.p, dv.p);
    }
    {
        st v{ { "\\", 3 } };
        auto dv = get_de(v);
        assert_val_equal(v.p, dv.p);
    }
    {
        st v{ { "<", 3 } };
        auto dv = get_de(v);
        assert_val_equal(v.p, dv.p);
    }
    {
        st v{ { ">", 3 } };
        auto dv = get_de(v);
        assert_val_equal(v.p, dv.p);
    }
    {
        st v{ { "<>", 3 } };
        auto dv = get_de(v);
        assert_val_equal(v.p, dv.p);
    }
    {
        st v{ { " ", 3 } };
        auto dv = get_de(v);
        assert_val_equal(v.p, dv.p);
    }
    {
        st v{ { "&", 3 } };
        auto dv = get_de(v);
        assert_val_equal(v.p, dv.p);
    }
    {
        st v{ { "\r", 3 } };
        auto dv = get_de(v);
        assert_val_equal(v.p, dv.p);
    }
    {
        st v{ { "\n", 3 } };
        auto dv = get_de(v);
        assert_val_equal(v.p, dv.p);
    }
    {
        st v{ { "\r\n", 3 } };
        auto dv = get_de(v);
        assert_val_equal(v.p, dv.p);
    }
}

TEST_F(base_type, struct_with_pair_with_chinese)
{
    struct st
    {
        std::pair<string, int> p;
        META(p);
    };
    {
        st v{ { "ÖÐÎÄ", 3 } };
        auto dv = get_de(v);
        assert_val_equal(v.p, dv.p);
    }
}