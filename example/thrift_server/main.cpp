#include "RpcHelloService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <network/gameserver/tcp_connect.hpp>
#include <network/gameserver/tcp_server.hpp>


using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

namespace cytx
{
    struct thrift_header
    {
        using this_t = thrift_header;
    private:
        static bool internal_big_endian(bool* is_big_endian = nullptr)
        {
            static bool _big_endian = false;
            if (is_big_endian)
                _big_endian = *is_big_endian;
            return _big_endian;
        }

    public:
        static bool big_endian()
        {
            return internal_big_endian();
        }

        static void big_endian(bool is_big_endian)
        {
            internal_big_endian(&is_big_endian);
        }

    public:

        uint32_t length;

        thrift_header() = default;

        thrift_header(uint32_t len)
        {
            init(len);
        }

        void ntoh()
        {
            if (!big_endian())
                return;

            length = cytx::util::ntoh(length);
        }

        void hton()
        {
            if (!big_endian())
                return;

            length = cytx::util::hton(length);
        }

        void init(uint32_t len)
        {
            this->length = len;
        }

        boost::asio::const_buffer to_buffer() const
        {
            return boost::asio::buffer(this, sizeof(this_t));
        }
    };

    using thrift_msg = gameserver::basic_msg<thrift_header, gameserver::msg_body>;

    using server_t = gameserver::tcp_server<thrift_msg>;
    using server_ptr = std::unique_ptr<server_t>;
    using connection_t = server_t::connection_t;
    using connection_ptr = server_t::connection_ptr;

    class thrift_server : public gameserver::irouter<connection_t>
    {
    public:
        void init()
        {
            gameserver::server_options options;
            options.batch_send_msg = true;
            options.disconnect_interval = 0;
            options.thread_mode = gameserver::server_thread_mode::no_io_thread;
            server_ = std::make_unique<server_t>("127.0.0.1", 6327, options, this);
        }

        void start()
        {
            server_->start();
        }

        virtual void on_connect(connection_ptr& conn_ptr, const cytx::net_result& err) override
        {
            if (err)
                return;
        }


        virtual void on_disconnect(connection_ptr& conn_ptr, const cytx::net_result& err) override
        {
            //log error
            //clear conn resource
        }


        virtual void on_receive(connection_ptr& conn_ptr, const msg_ptr& msgp) override
        {
            //process message
        }

    private:
        server_ptr server_;
    };
}


class RpcHelloServiceHandler : virtual public RpcHelloServiceIf {
public:
    RpcHelloServiceHandler() {
        // Your initialization goes here
    }

    int32_t show(const std::string& name) {
        // Your implementation goes here
        printf("show\n");
        return 0;
    }

};

int main(int argc, char **argv) {
    int port = 9090;
    shared_ptr<RpcHelloServiceHandler> handler(new RpcHelloServiceHandler());
    shared_ptr<TProcessor> processor(new RpcHelloServiceProcessor(handler));
    shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
    shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

    TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
    server.serve();
    return 0;
}