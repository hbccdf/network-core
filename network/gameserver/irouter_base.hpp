#pragma once
#include "network/base/exception.hpp"
namespace cytx
{
    namespace gameserver
    {
        template<typename CONN_T>
        class irouter_base
        {
        public:
            using conn_t = CONN_T;
            using connection_ptr = std::shared_ptr<conn_t>;
            using msg_t = typename conn_t::msg_t;
            using msg_ptr = typename conn_t::msg_ptr;

        public:
            virtual void on_connect(connection_ptr& conn_ptr, const cytx::net_result& err) {};
            virtual void on_disconnect(connection_ptr& conn_ptr, const cytx::net_result& err) {};

            virtual void on_receive(connection_ptr& conn_ptr, const msg_ptr& msgp) = 0;
            virtual void on_receive_msgs(connection_ptr& conn_ptr, const std::vector<msg_ptr>& msg_list) = 0;
        };

        template<typename CONN_T>
        class irouter : public irouter_base<CONN_T>
        {
        public:
            using base_t = irouter_base<CONN_T>;
            using conn_t = typename base_t::conn_t;
            using connection_ptr = typename base_t::connection_ptr;
            using msg_t = typename base_t::msg_t;
            using msg_ptr = typename base_t::msg_ptr;

            void on_receive_msgs(connection_ptr& conn_ptr, const std::vector<msg_ptr>& msg_list) override
            {
                for (auto& msgp : msg_list)
                {
                    this->on_receive(conn_ptr, msgp);
                }
            }
        };
    }
}