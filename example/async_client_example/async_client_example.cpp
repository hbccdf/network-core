#include <network/rpc/client.hpp>
#include <network/codec/codec.hpp>
#include <network/util/net.hpp>
#include <network/base/log.hpp>
#include <iostream>

namespace client
{
    DEFINE_STR_PROTOCOL(add, int(int, int));
    DEFINE_STR_PROTOCOL(add_with_conn, int(int, int));
    DEFINE_STR_PROTOCOL(sub_not_exist, double(int, std::string const&));
    DEFINE_STR_PROTOCOL(xml_add, int(int, int));
}

using tcp = boost::asio::ip::tcp;
using async_client_t = cytx::rpc::async_client<cytx::codec::json_codec>;
using connect_ptr = async_client_t::connection_ptr;
using router_t = typename async_client_t::router_t;
using irouter_t = typename async_client_t::irouter_t;
using irouter_ptr = typename async_client_t::irouter_ptr;

class client_router : public irouter_t
{
public:
    void on_connect(connection_ptr& conn_ptr, const cytx::net_result& err) override
    {
        std::cout << "on connect" << std::endl;
    }
    void on_receive(connection_ptr& ptr, const msg_ptr& msg) override
    {
        std::cout << "on msg" << std::endl;
    }
    void on_disconnect(connection_ptr& conn_ptr, const cytx::net_result& err) override
    {
        std::cout << "on disconnect" << std::endl;
    }
};

client_router _router;

// create the client
async_client_t asy_client(cytx::util::to_tcp_endpoint("127.0.0.1", 9000), &_router);

void async_client_rpc_example()
{
    using namespace std::chrono_literals;

    // the interface is type safe and non-connect oriented designed
    //asycn_client.call(endpoint, client::add, 1.0, 200.0f);

    asy_client.register_handler("sub", [](int a, int b) { std::cout << "sub " << a << "-" << b << std::endl; return a - b; });
    asy_client.register_handler("sub_with_conn", [](int a, connect_ptr conn, int b) { return a - b; });

    asy_client.call(client::add_with_conn, 9, 10);

    asy_client.call<cytx::codec::xml_codec>(client::xml_add, 1, 2).on_ok([](auto r)
    {
        std::cout << "xml " << r << std::endl;
    }).on_error([](auto const& error)
    {
        std::cout << error.message() << std::endl;
    });

    // we can set some callbacks to process some specific eventsS
    asy_client.call(client::add, 1, 2).on_ok([](auto r)
    {
        std::cout << r << std::endl;
    }).on_error([](auto const& error)
    {
        std::cout << error.message() << std::endl;
    })/*.timeout(1min)*/;

    // we can also use the asynchronized client in a synchronized way
    try
    {
        auto task = asy_client.call(client::add, 3, 5);
        auto const& result = task.get();
        std::cout << result << std::endl;
    }
    catch (cytx::net_exception const& e)
    {
        std::cout << e.message() << std::endl;
    }

    /*auto backup_endpoint = cytx::rpc::get_tcp_endpoint("127.0.0.1", 8999);
    asycn_client.call(endpoint, client::add, 1, 2).on_error([&backup_endpoint](auto const& error)
    {
        if (error.get_error_code() == cytx::rpc::error_code::BADCONNECTION)
            asycn_client.call(backup_endpoint, client::add, 1, 2);
    });*/
}


void test_timeout()
{
    using namespace std::chrono_literals;
    for (auto loop = 0; loop < 10000; ++loop)
    {
        asy_client.call(client::add, 1, loop).
            on_error([loop](auto const& error)
        {
            std::cout << "Error: " << error.message() << " loop" << loop << std::endl;
        }).on_ok([loop](auto r)
        {
            std::cout << "OK loop" << loop << std::endl;
        });
        std::this_thread::sleep_for(10s);
    }
}

void test_connect();

void on_connect(const cytx::net_result& ec)
{
    using namespace std::chrono_literals;
    if (!ec)
    {
        std::cout << " connect success" << std::endl;
    }
    else
    {
        std::cout << " connect failed, " << ec.message() << std::endl;
    }
}


void test_call();

void on_call(int r)
{
    std::cout << r << std::endl;
    test_call();
}


void test_delay();
void test_call()
{
    asy_client.call(client::add, 1, 2).on_ok(on_call)
        .on_error([](auto& ec)
    {
        test_delay();
    }).delay(1);
}

void test_connect()
{
    while (true)
    {
        auto task = asy_client.connect();
        auto ec = task.wait();
        if (!ec)
        {
            std::cout << " connect success" << std::endl;
        }
        else
        {
            std::cout << " connect failed, " << ec.message() << std::endl;
            continue;
        }

        while (true)
        {
            try
            {
                auto task = asy_client.call(client::add, 1, 23);
                auto r = task.get();
                std::cout << "add, " << r << std::endl;

                auto t1 = asy_client.free_call<std::string>("ping");
                auto r1 = t1.get();
                std::cout << r1 << std::endl;
            }
            catch (cytx::net_exception& e)
            {
                std::cout << e.message() << std::endl;
            }

            using namespace std::chrono_literals;
            auto task = asy_client.call(client::add, 66, 23);
            cytx::net_result e;
            auto r = task.get(e);
            if (r)
                std::cout << "add, " << r.value() << std::endl;
            else
            {
                std::cout << e.message() << std::endl;
            }

            try
            {
                asy_client.call(client::add_with_conn, 9, 100).on_ok([](auto r) {std::cout << "on ok, " << r << std::endl; })
                    .on_error([](auto& e) {std::cout << "on error, " << e.message() << std::endl; });
            }
            catch (std::exception& e)
            {
                std::cout << e.what() << std::endl;
            }
            catch (cytx::net_exception& e)
            {
                std::cout << "on error, " << e.message() << std::endl;
            }
        }
    }
}

void test_delay()
{
    asy_client.connect().on_ok([]
    {
        on_connect(cytx::net_result());
        test_call();
    })
     .on_error([](auto& ec)
    {
        on_connect(ec);
        test_delay();
    }).delay(std::chrono::seconds(3));
}

void test_timeout_func()
{
    auto task = asy_client.connect();
    auto ec = task.wait(std::chrono::milliseconds(50));
    std::cout << "result " << ec.message() << std::endl;

    auto t1 = asy_client.connect();
    auto ec1 = t1.wait();
    std::cout << "t1, " << ec1.message() << std::endl;

    while (true)
    {
        try
        {
            auto t2 = asy_client.free_call<std::string>("ping");
            auto r2 = t2.get(std::chrono::milliseconds(1));
            std::cout << r2 << std::endl;
        }
        catch (cytx::net_exception& e)
        {
            std::cout << e.message() << std::endl;
        }

        asy_client.free_call<std::string>("ping").on_ok([](std::string po) {std::cout << "free_call " << po << std::endl; })
            .on_error([](auto& ec) { std::cout << "free_call " << ec.message() << std::endl; }).timeout(std::chrono::milliseconds(1));

        asy_client.free_call<std::string>("ping").on_ok([](std::string po) {std::cout << "free_call2 " << po << std::endl; })
            .timeout(std::chrono::milliseconds(1));

        asy_client.free_call<std::string>("ping").on_error([](auto& ec) { std::cout << "free_call3 " << ec.message() << std::endl; })
            .timeout(std::chrono::milliseconds(1));

        asy_client.free_call<std::string>("ping").timeout(std::chrono::milliseconds(1));

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int main()
{
    cytx::memory_pool::ins().init();
    cytx::log::get().init("async_client_example.lg");
    /* async_client_rpc_example();
     test_timeout();*/
    //test_connect();
    //test_delay();
    test_timeout_func();
    std::getchar();
    return 0;
}