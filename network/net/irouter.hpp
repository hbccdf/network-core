#pragma once
#include <boost/asio.hpp>
#include "../base/common.h"

namespace cytx
{
    namespace rpc
    {
        template<typename connect_type>
        class irouter
        {
        public:
            using connection_t = connect_type;
            using connection_ptr = std::shared_ptr<connect_type>;
            using header_t = typename connection_t::header_t;

            virtual void connection_incoming(const boost::system::error_code& err, connection_ptr conn_ptr) {};
            virtual void message_received(connection_ptr& ptr, header_t& header, char* data, size_t size) = 0;
            virtual void connection_terminated(const boost::system::error_code& err, connection_ptr conn_ptr, cytx::rpc::error_code err_code) {};
        };
    }
}