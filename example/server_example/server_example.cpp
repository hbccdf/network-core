#include <server.hpp>
#include <codec/codec.hpp>
#include <base/utils.hpp>
#include <base/log.hpp>
#include <iostream>

uint16_t port = 9000;
size_t pool_size = std::thread::hardware_concurrency();

namespace client
{
    int add(int a, int b)
    {
        return a + b;
    }

    void dummy()
    {
        std::cout << "dummy" << std::endl;
    }

    void some_task_takes_a_lot_of_time(double, int)
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(5s);
    }

    struct foo
    {
        int add(int a, int b)
        {
            return a + b;
        }
    };

    DEFINE_STR_PROTOCOL(sub, int(int, int));
    DEFINE_STR_PROTOCOL(sub_with_conn, int(int, int));
}

template <size_t ... Is>
void print(std::index_sequence<Is...>)
{
    bool swallow[] = { (printf("%d\n", Is), true)... };
}

int main()
{
    cytx::log::get().init("rest_rpc_server.lg");
    using server_t = cytx::rpc::server<cytx::rpc::json_codec>;
    using connection_ptr = server_t::connection_ptr;
    server_t server{ port };
    client::foo foo{};

    connection_ptr client_conn = nullptr;

    server.register_handler("add", client::add);
    server.register_handler("dummy", client::dummy);
    server.register_handler("add_with_conn", [&client_conn](int a, connection_ptr conn, int b)
    {
        client_conn = conn;
        auto result = a + b;
        /*if (result < 1)
            conn->close();*/
        return result;
    });

    server.register_handler("time_consuming", client::some_task_takes_a_lot_of_time, [](auto conn) { std::cout << "acomplished!" << std::endl; });

    server.register_codec_handler<cytx::rpc::xml_codec>("xml_add", client::add);

    server.register_handler("ping", []() ->std::string { std::cout << "ping" << std::endl;  return "pong"; });

    server.start();

    while (true)
    {
        if (client_conn != nullptr)
        {
            try
            {
                auto task = client_conn->call(client::sub, 4, 1);
                auto result = task.get();
                std::cout << "client sub result = " << result << std::endl;
            }
            catch (cytx::rpc::rpc_exception& e)
            {
                std::cout << "client sub failed, " << e.message() << std::endl;
            }
        }
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(2s);
    }
    std::getchar();
    server.stop();
    return 0;
}