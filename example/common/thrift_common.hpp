#pragma once
#include "RpcHelloService.h"
#include <network/net/tcp_server.hpp>
#include <network/net/tcp_connect.hpp>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/processor/TMultiplexedProcessor.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

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

    using thrift_msg = net::basic_msg<thrift_header, net::msg_body>;

    using server_t = net::tcp_server<thrift_msg>;
    using server_ptr = std::unique_ptr<server_t>;
    using connection_t = server_t::connection_t;
    using connection_ptr = server_t::connection_ptr;
    using msg_t = server_t::msg_t;
    using msg_ptr = server_t::msg_ptr;
    using gos_t = memory_stream;
    using irouter_t = server_t::irouter_t;
    using irouter_ptr = server_t::irouter_ptr;
    using processer_ptr = boost::shared_ptr<TProcessor>;

    class thrift_transport : public TTransport
    {
    protected:
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

        const uint8_t* borrow_virt(uint8_t* buf, uint32_t* len) override
        {
            return (uint8_t*)reader_.data();
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

        bool isOpen() override
        {
            return ptr_->socket().is_open();
        }

        void close() override
        {
            ptr_->close();
        }

        virtual void set_current_msg(msg_ptr msg)
        {
            current_msg_ptr_ = msg;
            reader_ = msg->get_stream();
        }

    };

    using protocol_t = TBinaryProtocolT<thrift_transport>;
    using protocol_ptr = boost::shared_ptr<protocol_t>;
}