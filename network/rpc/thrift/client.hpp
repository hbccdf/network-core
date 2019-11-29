#pragma once
#include "network/base/waitable_object.hpp"
#include "network/net/ios_wrapper.hpp"
#include "transport.hpp"

namespace cytx
{
    namespace thrift
    {
        class thrift_client : public thrift_transport, public irouter_t
        {
            using base_t = thrift_transport;
            using ios_t = boost::asio::io_service;
        public:
            thrift_client(const std::string& host, uint32_t port)
                : base_t()
                , ios_()
                , host_(host)
                , port_(port)
            {
                init();
            }

            thrift_client(ios_t& ios, const std::string& host, uint32_t port)
                : base_t()
                , ios_(ios)
                , host_(host)
                , port_(port)
            {
                init();
            }

            ~thrift_client()
            {
                conn_ptr_->close();
                conn_ptr_ = nullptr;
                ios_.stop();
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
                log_->debug("client {}, begin connect tcp://{}:{}", conn_ptr_->get_conn_id(), host_, port_);
                net_result ret = conn_ptr_->connect(host_, port_);
                log_->debug("client {}, connect result {}", conn_ptr_->get_conn_id(), ret.message());
            }

            void on_connect(connection_ptr& conn_ptr, const cytx::net_result& err) override
            {
                log_->debug("client {}, connect result {}", conn_ptr_->get_conn_id(), err.message());
            }

            void on_disconnect(connection_ptr& conn_ptr, const cytx::net_result& err) override
            {
                log_->debug("client {}, disconnect, {}", conn_ptr_->get_conn_id(), err.message());
                waiter_.notify();
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

            tprotocol_ptr get_proto() const
            {
                return proto_;
            }

            tprotocol_ptr get_multi_proto(const std::string& service_name) const
            {
                boost::shared_ptr<tprotocol_t> ptr(new multi_protocol_t(proto_, service_name));
                return ptr;
            }

        protected:
            void init()
            {
                auto transport_ptr = boost::shared_ptr<thrift_client>(this, [](auto ptr) {});
                log_ = cytx::log::force_get_log("thrift");
                conn_ptr_ = std::make_shared<connection_t>(ios_.service(), this, 0, net::connection_options{ 0, true });

                conn_ptr_->world()["client"] = this;
                proto_ = boost::make_shared<TCompactProtocolT<thrift_transport>>(transport_ptr);

                ios_.start();
            }

        protected:
            net::ios_wrapper ios_;
            waitable_object waiter_;
            std::string host_;
            uint32_t port_;
            tprotocol_ptr proto_;
            log_ptr_t log_;
        };
    }
}