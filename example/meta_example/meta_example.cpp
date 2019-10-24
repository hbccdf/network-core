
#include <iostream>
#include <network/meta/meta.hpp>

using namespace std;
using namespace cytx;

class test
{
public:
    void show(char c)
    {
        cout << "show" << endl;
    }

    void show(int i)
    {
        cout << i << endl;
    }
};



inline auto to_method_extend(const test*)
{
    struct method_meta_test
    {
        //using meta_type = decltype();
        static auto Meta()
        {
            return std::make_tuple(
                std::make_pair("show_char", (void(test::*)(char))&test::show),
                std::make_pair("show_int", (void(test::*)(int))&test::show)
            );
        }
    };

    return method_meta_test{};
}

template<typename T>
struct get_arg
{};

template<typename T, typename Arg>
struct get_arg<void(T::*)(Arg)>
{
    using type = Arg;
};

int main()
{
    auto meta_val = to_method_extend((test*)nullptr);

    using met_t = decltype(meta_val);

    auto met = met_t::Meta();

    cytx::for_each(met, [](auto& p, size_t i, bool is_last) {
        std::cout << p.first << std::endl;

        using func_t = std::decay_t<decltype(p.second)>;


        using arg_type = typename get_arg<func_t>::type;

        cout << typeid(arg_type).name() << endl;
    });

    return 0;
}