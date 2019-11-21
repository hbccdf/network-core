#include "thrift_common.hpp"
#include "network/base/waitable_object.hpp"
#include "network/rpc/net/ios_wrapper.hpp"

namespace cytx
{
    using ios_t = boost::asio::io_service;

    class thrift_client_transport : public thrift_transport, public irouter_t
    {
        using base_t = thrift_transport;
    private:
        waitable_object waiter_;
        std::string host_;
        uint32_t port_;

    public:
        thrift_client_transport(ios_t& ios, const std::string& host, uint32_t port)
            : base_t(std::make_shared<connection_t>(ios, this, 0, gameserver::connection_options{30, true}))
            , host_(host)
            , port_(port)
        {
            ptr_->world()["client"] = this;
        }

        uint32_t read_virt(uint8_t* buffer, uint32_t length) override
        {
            if (current_msg_ptr_ == nullptr)
            {
                waiter_.wait();
            }

            if (current_msg_ptr_ == nullptr)
                return -1;

            return base_t::read_virt(buffer, length);
        }

        void set_current_msg(msg_ptr msg) override
        {
            base_t::set_current_msg(msg);
            waiter_.notify();
        }

        void open() override
        {
            ptr_->connect(host_, port_);
        }

        void on_connect(connection_ptr& conn_ptr, const cytx::net_result& err) override
        {
        }

        void on_disconnect(connection_ptr& conn_ptr, const cytx::net_result& err) override
        {
        }

        void on_receive(connection_ptr& conn_ptr, const msg_ptr& msgp) override
        {
            set_current_msg(msgp);
        }

        uint32_t writeEnd() override
        {
            if (!isOpen())
            {
                open();
            }

            if (!isOpen())
                return -1;

            return base_t::writeEnd();
        }

    };

    class thrift_manager
    {
    private:
        rpc::ios_wrapper ios_;
    public:
        void start()
        {
            ios_.start();
        }

        ios_t& get_ios()
        {
            return ios_.service();
        }
    };
}

int main(int argc, char* argv[])
{
    cytx::log::init_log(cytx::log_level_t::debug, "net");
    cytx::log::init_log(cytx::log_level_t::debug, "conn");

    cytx::thrift_manager manager;

    auto transport_ptr = boost::make_shared<cytx::thrift_client_transport>(manager.get_ios(), "127.0.0.1", 6327);

    auto proto = boost::make_shared<cytx::protocol_t>(transport_ptr);

    RpcHelloServiceClient client(proto);

    manager.start();
    int result = client.show("test");

    std::cout << result << std::endl;

    return 0;
}