#include "common.h"

#define ENUM_META_RALAX_CHECK
#include <network/serialize.hpp>

using namespace cytx;
using namespace std;

int ______init = ([]()->int { memory_pool::ins().init(); return 1; })();

class gos_type : public ::testing::Test
{
public:
    memory_stream gos;
    Serializer<gos_serialize_adapter> se{ gos };
    DeSerializer<gos_deserialize_adapter> de{ gos };

    virtual void SetUp() {

    }

    virtual void TearDown() {}

    template<typename T>
    auto assert_val_equal(T&& v, T&& dv)-> std::enable_if_t<!std::is_floating_point<T>::value>
    {
        assert_eq(v, dv);
    }
    template<typename T>
    auto assert_val_equal(T&& v, T&& dv) -> std::enable_if_t<std::is_same<T, float>::value>
    {
        assert_float_eq(v, dv);
    }
    template<typename T>
    auto assert_val_equal(T&& v, T&& dv) -> std::enable_if_t<std::is_same<T, double>::value>
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
};

int func_test(int a, int b)
{
    return a + b;
}

struct foo
{
    int add(int a, int b)
    {
        return a + b;
    }
};

TEST(test, t)
{
    auto f = cytx::bind(func_test);
    auto r = f(2, 9);
    assert_eq(r, 11);

    foo ff;
    auto f1 = cytx::bind(&foo::add, &ff);

    auto ff2 = std::make_shared<foo>();
    auto f2 = cytx::bind(&foo::add, ff2);

    auto r1 = f1(2, 4);
    auto r2 = f2(2, 4);
    assert_eq(r1, 6);
    assert_eq(r2, 6);

    /*auto f3 = cytx::bind(&foo::add, ff2, std::placeholders::_1, 3);
    auto r3 = f3(4);
    assert_eq(r3, 7);*/
}

TEST_F(gos_type, bool)
{
    bool v = true;
    se.Serialize(v);
    bool dv;
    de.DeSerialize(dv);
    assert_eq(v, dv);
}

TEST_F(gos_type, char)
{
    char v = 9;
    se.Serialize(v);
    char dv;
    de.DeSerialize(dv);
    assert_eq(v, dv);
}

TEST_F(gos_type, uchar)
{
    unsigned char v = 150;
    se.Serialize(v);
    unsigned char dv;
    de.DeSerialize(dv);
    assert_eq(v, dv);
}

TEST_F(gos_type, short)
{
    short v = 159;
    se.Serialize(v);
    short dv;
    de.DeSerialize(dv);
    assert_eq(v, dv);
}

TEST_F(gos_type, ushort)
{
    unsigned short v = 40000;
    se.Serialize(v);
    unsigned short dv;
    de.DeSerialize(dv);
    assert_eq(v, dv);
}

TEST_F(gos_type, int)
{
    int v = 159 * 1024;
    se.Serialize(v);
    int dv;
    de.DeSerialize(dv);
    assert_eq(v, dv);
}

TEST_F(gos_type, uint)
{
    unsigned int v = 159 * 1024;
    se.Serialize(v);
    unsigned int dv;
    de.DeSerialize(dv);
    assert_eq(v, dv);
}

TEST_F(gos_type, float)
{
    float v = 9.1f;
    se.Serialize(v);
    float dv;
    de.DeSerialize(dv);
    assert_float_eq(v, dv);
}

TEST_F(gos_type, double)
{
    double v = 9.999345;
    se.Serialize(v);
    double dv;
    de.DeSerialize(dv);
    assert_double_eq(v, dv);
}

TEST_F(gos_type, string)
{
    string v = "this is a test";
    se.Serialize(v);
    string dv;
    de.DeSerialize(dv);
    assert_streq(v.c_str(), dv.c_str());
}

template<typename T, size_t N>
void assert_array(T(&v1)[N], T(&v2)[N])
{
    for (size_t i = 0; i < N; ++i)
    {
        assert_eq(v1[i], v2[i]) << "i = " << i;
    }
}

template<size_t N>
void assert_array(float(&v1)[N], float(&v2)[N])
{
    for (size_t i = 0; i < N; ++i)
    {
        assert_float_eq(v1[i], v2[i]) << "i = " << i;
    }
}

template<size_t N>
void assert_array(double(&v1)[N], double(&v2)[N])
{
    for (size_t i = 0; i < N; ++i)
    {
        assert_double_eq(v1[i], v2[i]) << "i = " << i;
    }
}

template<size_t N>
void assert_array(char(&v1)[N], char(&v2)[N])
{
    assert_streq(v1, v2);
}

TEST_F(gos_type, base_array_int)
{
    int v[2] = { 3, 9 };
    se.Serialize(v);
    int dv[2];
    de.DeSerialize(dv);
    assert_array(v, dv);
}

TEST_F(gos_type, base_array_char)
{
    char v[10] = { "hello" };
    se.Serialize(v);
    char dv[10];
    de.DeSerialize(dv);
    assert_array(v, dv);
}

TEST_F(gos_type, base_array_float)
{
    float v[4] = { 3.1f, 9.4f, -11.1f, 9999.3f };
    se.Serialize(v);
    float dv[4];
    de.DeSerialize(dv);
    assert_array(v, dv);
}

TEST_F(gos_type, base_array_double)
{
    double v[5] = { 3.1, 9.4, -11.1, 9999.3, 0.0000324 };
    se.Serialize(v);
    double dv[5];
    de.DeSerialize(dv);
    assert_array(v, dv);
}

TEST_F(gos_type, std_array)
{
    std::array<int, 3> v = { 1, 2, 3 };
    se.Serialize(v);
    std::array<int, 3> dv;
    de.DeSerialize(dv);
    for (size_t i = 0; i < 3; i++)
    {
        assert_eq(v[i], dv[i]);
    }
}

TEST_F(gos_type, tuple)
{
    std::tuple<int, int> v(3, 9);
    std::tuple<int, int> dv;
    se.Serialize(v);
    de.DeSerialize(dv);
    assert_eq(std::get<0>(v), std::get<0>(dv));
    assert_eq(std::get<1>(v), std::get<1>(dv));
}

TEST_F(gos_type, pair)
{
    std::pair<int, int> v = { 3, 9 };
    std::pair<int, int> dv;
    se.Serialize(v);
    de.DeSerialize(dv);
    assert_eq(v.first, dv.first);
    assert_eq(v.second, dv.second);
}

TEST_F(gos_type, vector)
{
    std::vector<int> v = { 3, 9 };
    std::vector<int> dv;
    se.Serialize(v);
    de.DeSerialize(dv);
    for (size_t i = 0; i < v.size(); i++)
    {
        assert_eq(v[i], dv[i]);
    }
}

TEST_F(gos_type, list)
{
    std::list<int> v = { 3, 9 };
    std::list<int> dv;
    se.Serialize(v);
    de.DeSerialize(dv);
    auto v_it = v.begin();
    auto dv_it = dv.begin();
    while (v_it != v.end())
    {
        assert_eq(*v_it, *dv_it);
        ++v_it;
        ++dv_it;
    }
}

TEST_F(gos_type, set)
{
    std::set<int> v = { 3, 9 };
    std::set<int> dv;
    se.Serialize(v);
    de.DeSerialize(dv);
    auto v_it = v.begin();
    auto dv_it = dv.begin();
    while (v_it != v.end())
    {
        assert_eq(*v_it, *dv_it);
        ++v_it;
        ++dv_it;
    }
}

TEST_F(gos_type, unordered_set)
{
    std::unordered_set<int> v = { 3, 9 };
    std::unordered_set<int> dv;
    std::set<int> vs;
    std::set<int> dvs;
    se.Serialize(v);
    de.DeSerialize(dv);

    for (auto i : v)
    {
        vs.insert(i);
    }
    for (auto i : dv)
    {
        dvs.insert(i);
    }

    auto v_it = vs.begin();
    auto dv_it = dvs.begin();
    while (v_it != vs.end())
    {
        assert_eq(*v_it, *dv_it);
        ++v_it;
        ++dv_it;
    }
}

TEST_F(gos_type, map)
{
    std::map<int, int> v = { { 3, 9 }, {2,4} };
    std::map<int, int> dv;
    se.Serialize(v);
    de.DeSerialize(dv);
    auto v_it = v.begin();
    auto dv_it = dv.begin();
    while (v_it != v.end())
    {
        assert_eq(v_it->first, dv_it->first);
        assert_eq(v_it->second, dv_it->second);
        ++v_it;
        ++dv_it;
    }
}

TEST_F(gos_type, unordered_map)
{
    std::unordered_map<int, int> v = { { 3, 9 },{ 2,4 } };
    std::unordered_map<int, int> dv;
    std::map<int, int> vs;
    std::map<int, int> dvs;

    se.Serialize(v);
    de.DeSerialize(dv);

    for (auto& i : v)
    {
        vs.insert(i);
    }
    for (auto& i : dv)
    {
        dvs.insert(i);
    }

    auto v_it = vs.begin();
    auto dv_it = dvs.begin();
    while (v_it != vs.end())
    {
        assert_eq(v_it->first, dv_it->first);
        assert_eq(v_it->second, dv_it->second);
        ++v_it;
        ++dv_it;
    }
}

TEST_F(gos_type, struct_bool)
{
    struct st
    {
        bool val;
        META(val);
    };
    st v = { true };
    se.Serialize(v);
    st dv;
    de.DeSerialize(dv);
    assert_eq(v.val, dv.val);
}

TEST_F(gos_type, struct_more_field)
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
    se.Serialize(v);
    st dv;
    de.DeSerialize(dv);
    assert_eq(v.val, dv.val);
    assert_eq(v.vi, dv.vi);
    assert_float_eq(v.vf, dv.vf);
    assert_streq(v.vs.c_str(), dv.vs.c_str());
    assert_streq(v.vcs, dv.vcs);
}

TEST_F(gos_type, struct_with_vector)
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
    st v = { true,  13, 2.2f, "this is a test for meta", "hello world", {3, 9, 100} };
    se.Serialize(v);
    st dv;
    de.DeSerialize(dv);
    assert_eq(v.val, dv.val);
    assert_eq(v.vi, dv.vi);
    assert_float_eq(v.vf, dv.vf);
    assert_streq(v.vs.c_str(), dv.vs.c_str());
    assert_streq(v.vcs, dv.vcs);
    for (size_t i = 0; i < v.vv.size(); i++)
    {
        assert_eq(v.vv[i], dv.vv[i]);
    }
}

TEST_F(gos_type, struct_with_list)
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
    se.Serialize(v);
    st dv;
    de.DeSerialize(dv);
    assert_eq(v.val, dv.val);
    assert_eq(v.vi, dv.vi);
    assert_float_eq(v.vf, dv.vf);
    assert_streq(v.vs.c_str(), dv.vs.c_str());
    assert_streq(v.vcs, dv.vcs);

    auto v_it = v.vls.begin();
    auto dv_it = dv.vls.begin();
    while (v_it != v.vls.end())
    {
        assert_eq(*v_it, *dv_it);
        ++v_it;
        ++dv_it;
    }
}

TEST_F(gos_type, struct_with_map)
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
    st v = { true,  13, { {2, "name"}, {9, "test"}, {-1, "age"} }, 2.2f, "this is a test for meta", "hello world" };
    se.Serialize(v);
    st dv;
    de.DeSerialize(dv);
    assert_eq(v.val, dv.val);
    assert_eq(v.vi, dv.vi);
    assert_float_eq(v.vf, dv.vf);
    assert_streq(v.vs.c_str(), dv.vs.c_str());
    assert_streq(v.vcs, dv.vcs);

    auto v_it = v.vm.begin();
    auto dv_it = dv.vm.begin();
    while (v_it != v.vm.end())
    {
        assert_eq(v_it->first, dv_it->first);
        assert_streq(v_it->second.c_str(), dv_it->second.c_str());
        ++v_it;
        ++dv_it;
    }
}


TEST_F(gos_type, struct_with_struct)
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
    st v = { true,  13,{ { 2, "name" },{ 9, "test" },{ -1, "age" } }, 2.2f, "this is a test for meta", "hello world", {3, "demo"} };
    se.Serialize(v);
    st dv;
    de.DeSerialize(dv);
    assert_eq(v.val, dv.val);
    assert_eq(v.vi, dv.vi);
    assert_float_eq(v.vf, dv.vf);
    assert_streq(v.vs.c_str(), dv.vs.c_str());
    assert_streq(v.vcs, dv.vcs);

    auto v_it = v.vm.begin();
    auto dv_it = dv.vm.begin();
    while (v_it != v.vm.end())
    {
        assert_eq(v_it->first, dv_it->first);
        assert_streq(v_it->second.c_str(), dv_it->second.c_str());
        ++v_it;
        ++dv_it;
    }

    assert_eq(v.vp.age, dv.vp.age);
    assert_streq(v.vp.name.c_str(), dv.vp.name.c_str());
}

TEST_F(gos_type, struct_with_struct_with_struct)
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
    st v = { true,  13,{ { 2, "name" },{ 9, "test" },{ -1, "age" } }, 2.2f, "this is a test for meta", "hello world",{ 3, "demo", { -100.235f, 10}} };
    se.Serialize(v);
    st dv;
    de.DeSerialize(dv);
    assert_eq(v.val, dv.val);
    assert_eq(v.vi, dv.vi);
    assert_float_eq(v.vf, dv.vf);
    assert_streq(v.vs.c_str(), dv.vs.c_str());
    assert_streq(v.vcs, dv.vcs);

    auto v_it = v.vm.begin();
    auto dv_it = dv.vm.begin();
    while (v_it != v.vm.end())
    {
        assert_eq(v_it->first, dv_it->first);
        assert_streq(v_it->second.c_str(), dv_it->second.c_str());
        ++v_it;
        ++dv_it;
    }

    assert_eq(v.vp.age, dv.vp.age);
    assert_streq(v.vp.name.c_str(), dv.vp.name.c_str());
    assert_float_eq(v.vp.t.f, dv.vp.t.f);
    assert_eq(v.vp.t.c, dv.vp.t.c);
}

TEST_F(gos_type, enum)
{
    enum en { val = 1, val2 = 4 };
    en v = val2;
    en dv;
    se.Serialize(v);
    de.DeSerialize(dv);
    assert_eq(v, dv);
}

TEST_F(gos_type, enum_class)
{
    enum class en { val = 1, val2 = 4 };
    en v = en::val2;
    en dv;
    se.Serialize(v);
    de.DeSerialize(dv);
    assert_eq(v, dv);
}

TEST_F(gos_type, enum_class_short)
{
    enum class en : short { val = 1, val2 = 4 };
    en v = en::val2;
    en dv;
    se.Serialize(v);
    de.DeSerialize(dv);
    assert_eq(v, dv);
}

TEST_F(gos_type, enum_class_short_with_struct)
{
    enum class en : short { val = 1, val2 = 4 };
    struct st
    {
        int vi;
        en ve;
        META(vi, ve);
    };

    st v = { 3, en::val };
    st dv;
    se.Serialize(v);
    de.DeSerialize(dv);
    assert_eq(v.vi, dv.vi);
    assert_eq(v.ve, dv.ve);
}

TEST_F(gos_type, other_tupe1)
{
    struct st
    {
        int vi;
        float vf;
        META(vi, vf);
    };

    st v{ 3, 2.2f };
    se.Serialize(v);

    DeSerializer<gos_deserialize_adapter, std::tuple<string>> de1{ std::tuple<string>{ "hello" }, gos };
    auto dv = de1.GetTuple<std::tuple<string, int, float>>();

    assert_streq(std::get<0>(dv).c_str(), "hello");
    assert_eq(std::get<1>(dv), v.vi);
    assert_float_eq(std::get<2>(dv), v.vf);
}

TEST_F(gos_type, other_tupe2_with_ref)
{
    struct st
    {
        int vi;
        float vf;
        META(vi, vf);
    };

    st v{ 3, 2.2f };
    se.Serialize(v);

    double d = 9.932;
    DeSerializer<gos_deserialize_adapter, std::tuple<double&>> de1{ std::tuple<double&>{ d }, gos };
    auto dv = de1.GetTuple<std::tuple<double&, int, float>>();

    assert_double_eq(std::get<0>(dv), d);
    assert_eq(std::get<1>(dv), v.vi);
    assert_float_eq(std::get<2>(dv), v.vf);

    d = 2.345;
    assert_double_eq(std::get<0>(dv), d);
}

enum struct_enum_key
{
    key1,
    key2,
    key3,
};
REG_ENUM(struct_enum_key, key1, key2, key3);

TEST_F(gos_type, struct_with_enum_key_map)
{
    struct st
    {
        std::map<struct_enum_key, int> enum_map;
        META(enum_map);
    };

    st v;
    v.enum_map.emplace(key1, 1);
    v.enum_map.emplace(key2, 2);
    v.enum_map.emplace(key3, 3);
    v.enum_map.emplace((struct_enum_key)9, 9);

    se.Serialize(v);
    st dv;
    de.DeSerialize(dv);
    assert_val_equal(v.enum_map, dv.enum_map);
}

TEST_F(gos_type, struct_with_optional)
{
    struct st
    {
        boost::optional<int> v;
        META(v);
    };
    st v;
    v.v = 3;
    se.Serialize(v);
    st dv;
    de.DeSerialize(dv);
    assert_val_equal(v.v, dv.v);
}

TEST_F(gos_type, struct_with_optional_null)
{
    struct st
    {
        boost::optional<int> v;
        META(v);
    };
    st v;
    se.Serialize(v);
    st dv;
    de.DeSerialize(dv);
    assert_val_equal(v.v, dv.v);
}

TEST_F(gos_type, struct_with_optional_and_other_field)
{
    struct st
    {
        int i;
        boost::optional<int> v;
        std::string str;
        META(i, v, str);
    };
    st v;
    v.i = 100;
    v.str = "test";
    se.Serialize(v);
    st dv;
    de.DeSerialize(dv);
    assert_val_equal(v.i, dv.i);
    assert_val_equal(v.v, dv.v);
    assert_val_equal(v.str, dv.str);

    gos.reset();

    v.v = 33;
    se.Serialize(v);
    de.DeSerialize(dv);
    assert_val_equal(v.i, dv.i);
    assert_val_equal(v.v, dv.v);
    assert_val_equal(v.str, dv.str);
}

TEST_F(gos_type, struct_with_optional_null_struct)
{
    struct base_st
    {
        std::string str;
        int i;
        boost::optional<float> v;
        double d;
        META(str, i, v, d);
    };
    struct st
    {
        int i;
        boost::optional<base_st> v;
        std::string str;
        META(i, v, str);
    };

    st v;
    v.i = 3;
    v.str = "test";
    se.Serialize(v);
    st dv;
    de.DeSerialize(dv);
    assert_val_equal(v.i, dv.i);
    assert_true(!dv.v);
    assert_val_equal(v.str, dv.str);
}

TEST_F(gos_type, struct_with_optional_struct)
{
    struct base_st
    {
        std::string str;
        int i;
        boost::optional<float> v;
        double d;
        META(str, i, v, d);
    };
    struct st
    {
        int i;
        boost::optional<base_st> v;
        std::string str;
        META(i, v, str);
    };

    base_st b;
    b.str = "hello";
    b.i = 10;
    b.v = 3.3f;
    b.d = 15.456;

    st v;
    v.i = 3;
    v.v = b;
    v.str = "test";
    se.Serialize(v);
    st dv;
    de.DeSerialize(dv);
    assert_val_equal(v.i, dv.i);
    assert_false(!dv.v);
    assert_val_equal(v.v->str, dv.v->str);
    assert_val_equal(v.v->i, dv.v->i);
    assert_val_equal(v.v->v, dv.v->v);
    assert_val_equal(v.v->d, dv.v->d);
    assert_val_equal(v.str, dv.str);
}