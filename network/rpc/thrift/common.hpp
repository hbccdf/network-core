#pragma once
#include <network/net/tcp_server.hpp>
#include <network/net/tcp_connect.hpp>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/processor/TMultiplexedProcessor.h>
#include <thrift/protocol/TMultiplexedProtocol.h>
#include "msg.hpp"

namespace cytx
{
    namespace thrift
    {
        using namespace ::apache::thrift;
        using namespace ::apache::thrift::protocol;
        using namespace ::apache::thrift::transport;

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
        using tprotocol_t = TProtocol;
        using tprotocol_ptr = boost::shared_ptr<tprotocol_t>;
        using multi_protocol_t = TMultiplexedProtocol;
        using multi_processer_t = TMultiplexedProcessor;
    }
}