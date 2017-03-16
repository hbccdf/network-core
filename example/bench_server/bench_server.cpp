#include <network/server.hpp>

namespace cytx
{
    enum class type_t
    {
        connection,
        operation,
    };
    REG_ENUM(type_t, connection, operation);

    struct config
    {
        string ip;
        uint16_t port;
        type_t test_type;
        META(ip, port, test_type);
    };
}

namespace bench
{
    int add(int a, int b)
    {
        return a + b;
    }

    template <typename Server>
    void count_qps(Server& server, std::atomic<uint64_t>& qps)
    {
        server.register_handler("add", bench::add,
            [&qps](auto, auto)
        {
            ++qps;
        });

        std::thread{ [&qps]
        {
            while (true)
            {
                using namespace std::chrono_literals;

                std::cout << "QPS: " << qps.load() << ".\n";
                qps.store(0);
                std::this_thread::sleep_for(1s);
            }

        } }.detach();
    }

    template <typename Server>
    void count_connection(Server& server, std::atomic<uint64_t>& conn_count)
    {
        server.register_handler("bench_connection", [&conn_count]()
        {
            ++conn_count;
        });

        std::thread{ [&conn_count]
        {
            while (true)
            {
                using namespace std::chrono_literals;

                std::cout << "Connection: " << conn_count.load() << ".\n";
                std::this_thread::sleep_for(1s);
            }

        } }.detach();
    }
}

int main(int argc, char *argv[])
{
    using namespace bench;
    using namespace std::chrono_literals;
    using namespace cytx;
    using namespace cytx::rpc;
    using server_t = cytx::rpc::server<cytx::rpc::json_codec>;
    //MemoryPoolManager::get_mutable_instance().init();
    config c;
    DeSerializer<xml_deserialize_adapter> de;
    de.enum_with_str(true);
    de.parse_file("bench.xml");
    de.DeSerialize(c, "config.server");

    std::atomic<uint64_t> work_count{ 0 };

    server_t server{ c.ip, c.port };

    switch (c.test_type)
    {
    case cytx::type_t::connection:
        bench::count_connection(server, work_count);
        break;
    case  cytx::type_t::operation:
        bench::count_qps(server, work_count);
        break;
    }

    server.start();
    std::getchar();
    server.stop();
    return 0;
}
