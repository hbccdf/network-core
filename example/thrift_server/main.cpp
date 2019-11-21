#include "RpcHelloService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <network/gameserver/tcp_connect.hpp>
#include <network/gameserver/tcp_server.hpp>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/processor/TMultiplexedProcessor.h>


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
    using msg_t = server_t::msg_t;
    using msg_ptr = server_t::msg_ptr;
    using gos_t = memory_stream;
    using processer_ptr = boost::shared_ptr<TProcessor>;

    class thrift_transport : public TTransport
    {
    private:
        connection_ptr ptr_;
        msg_ptr current_msg_ptr_;
        gos_t reader_{ 0 };
        gos_t writer_{ 10240 };
    public:
        thrift_transport(connection_ptr conn_ptr)
            : ptr_(conn_ptr)
        {}

        uint32_t read_virt(uint8_t* buffer, uint32_t length) override
        {
            reader_.read_binary((char*)buffer, length);
            return length;
        }

        uint32_t readAll_virt(uint8_t* buf, uint32_t len) override
        {
            return read_virt(buf, len);
        }

        void write_virt(const uint8_t* buffer, uint32_t length) override
        {
            writer_.write_binary((const char*)const_cast<uint8_t*>(buffer), length);
        }

        const uint8_t* borrow_virt(uint8_t*, uint32_t*) override
        {
            throw std::logic_error("The method or operation is not implemented.");
        }

        void consume_virt(uint32_t length) override
        {
            reader_.rd_ptr(length);
        }

        uint32_t readEnd() override
        {
            current_msg_ptr_ = nullptr;
            return 0;
        }

        uint32_t writeEnd() override
        {
            msg_ptr msg = std::make_shared<msg_t>(writer_);
            ptr_->write(msg);

            writer_.reallocate(10240);
            return 0;
        }

        void set_current_msg(msg_ptr msg)
        {
            current_msg_ptr_ = msg;
            reader_ = msg->get_stream();
        }

        bool isOpen() override
        {
            return ptr_->socket().is_open();
        }

        void open() override
        {
            //ptr_->connect();
        }


        void close() override
        {
            ptr_->close();
        }

    };

    using protocol_t = TCompactProtocolT<thrift_transport>;
    using protocol_ptr = boost::shared_ptr<protocol_t>;

    class thrift_server : public gameserver::irouter<connection_t>
    {
    public:
        thrift_server(processer_ptr processer)
            : processer_(processer)
        {}

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

        void on_connect(connection_ptr& conn_ptr, const cytx::net_result& err) override
        {
            if (err)
                return;

            auto transport = boost::make_shared<thrift_transport>(conn_ptr);
            protocol_t* proto_ptr = new protocol_t(transport);
            conn_ptr->world()["proto"] = proto_ptr;
        }

        void on_disconnect(connection_ptr& conn_ptr, const cytx::net_result& err) override
        {
            //log error
            protocol_t* proto_ptr = conn_ptr->world()["proto"];
            if (proto_ptr != nullptr)
            {
                delete proto_ptr;
                proto_ptr = nullptr;
                conn_ptr->world()["proto"] = proto_ptr;
            }
        }

        void on_receive(connection_ptr& conn_ptr, const msg_ptr& msgp) override
        {
            protocol_t* proto_ptr = conn_ptr->world()["proto"];
            if (proto_ptr == nullptr)
            {
                //log
            }

            protocol_ptr proto(proto_ptr);

            processer_->process(proto, proto, nullptr);
        }

    private:
        server_ptr server_;
        processer_ptr processer_;
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
    /*shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
    shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

    TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
    server.serve();*/

    cytx::log::init_log(cytx::log_level_t::debug, "net");

    cytx::thrift_server server(processor);
    server.init();
    server.start();

    return 0;
}