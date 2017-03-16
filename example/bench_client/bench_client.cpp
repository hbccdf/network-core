#include <network/client.hpp>

namespace cytx
{
    enum class type_t
    {
        connection,
        operation,
    };
    REG_ENUM(type_t, connection, operation);

    enum class sync_t
    {
        async,
        sync,
    };
    REG_ENUM(sync_t, async, sync);

    struct config
    {
        string ip;
        uint16_t port;
        type_t test_type;
        sync_t sync_type;
        META(ip, port, test_type, sync_type);
    };
}

namespace client
{
    DEFINE_STR_PROTOCOL(add, void(int, int));
}

namespace bench
{
    std::atomic<uint64_t> success_count{ 0 }, failed_count{ 0 };
    uint64_t send_count{ 0 }, recv_count{ 0 };

    void bench_async(boost::asio::ip::tcp::endpoint const& endpoint)
    {
        using client_t = cytx::rpc::async_client<cytx::rpc::json_codec>;

        auto client = std::make_shared<client_t>(endpoint);

        std::thread{ []
        {
            while (true)
            {
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(1s);
                std::cout << "success " << success_count.load() << ", failed " << failed_count.load();
                std::cout << ", send " << send_count << ", recv " << recv_count << std::endl;
                success_count.store(0);
                failed_count.store(0);
            }
        } }.detach();


        std::thread{
            [client, &endpoint]
        {
            int a = 0, b = 0, sum = 0;
            while (true)
            {
                client->free_call<void>("add", a, b++).on_ok([]()
                {
                    ++success_count;
                    ++recv_count;
                }).on_error([](auto) { ++failed_count; ++recv_count; });
                ++send_count;
                
                if (send_count % 100000 == 0)
                {
                    using namespace std::chrono_literals;
                    std::this_thread::sleep_for(2s);
                }
            }

        } }.detach();
    }

    void bench_sync(boost::asio::ip::tcp::endpoint const& endpoint)
    {
        using client_t = cytx::rpc::sync_client<cytx::rpc::json_codec>;

        std::thread{ []
        {
            while (true)
            {
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(1s);
                std::cout << success_count.load() << std::endl;
                success_count.store(0);
            }
        } }.detach();

        std::thread{
            [endpoint]
        {
            client_t client(endpoint);

            int a = 0, b = 0;
            while (true)
            {
                try
                {
                    client.call(client::add, a, b++);
                    ++success_count;
                }
                catch (...)
                {
                    std::cout << "Exception: " << std::endl;
                    break;
                }
            }

        } }.detach();
    }
}

int main(int argc, char *argv[])
{
    using namespace cytx;
    using namespace cytx::rpc; 
    //MemoryPoolManager::get_mutable_instance().init();
    config c;
    DeSerializer<xml_deserialize_adapter> de;
    de.enum_with_str(true);
    de.parse_file("bench.xml");
    de.DeSerialize(c, "config.client");

    auto endpoint = cytx::rpc::get_tcp_endpoint(c.ip, c.port);
    if (c.sync_type == sync_t::async)
    {
        bench::bench_async(endpoint);
    }
    else
    {
        bench::bench_sync(endpoint);
    }

    std::getchar();
    return 0;
}
