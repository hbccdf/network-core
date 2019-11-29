#pragma once
#include "transport.hpp"

namespace cytx
{
    namespace thrift
    {
        using net::server_thread_mode;

        struct thrift_server_options
        {
            processer_ptr processer = nullptr;
            std::string ip = "0.0.0.0";
            uint16_t port = 0;
            server_thread_mode thread_mode = server_thread_mode::no_io_thread;
            int disconnect_interval = 30;

            thrift_server_options(processer_ptr processer, const std::string& ip, uint16_t port, server_thread_mode mode, int interval)
            {
                this->processer = processer;
                this->ip = ip;
                this->port = port;
                this->thread_mode = mode;
                this->disconnect_interval = interval;
            }
        };

        class thrift_server : public irouter_t
        {
        public:
            thrift_server(processer_ptr processer, uint16_t port)
                : thrift_server(processer, "0.0.0.0", port)
            {}

            thrift_server(processer_ptr processer, const std::string& ip, uint16_t port)
                : thrift_server(processer, ip, port, server_thread_mode::no_io_thread)
            {
            }

            thrift_server(processer_ptr processer, const std::string& ip, uint16_t port, server_thread_mode mode)
                : thrift_server(processer, ip, port, mode, 30)
            {
            }

            thrift_server(processer_ptr processer, const std::string& ip, uint16_t port, server_thread_mode mode, int disconnect_interval)
                : thrift_server(thrift_server_options{ processer, ip, port, mode, disconnect_interval })
            {
            }

            thrift_server(const thrift_server_options& options)
            {
                init(options);
            }

            void start()
            {
                server_->start();
            }

            void on_connect(connection_ptr& conn_ptr, const cytx::net_result& err) override
            {
                if (err)
                {
                    log_->debug("accept connect error {}", err.message());
                    return;
                }

                log_->debug("new conn {}", conn_ptr->get_conn_id());

                auto transport = boost::make_shared<thrift_transport>(conn_ptr);
                tprotocol_t* proto_ptr = new TCompactProtocolT<thrift_transport>(transport);
                conn_ptr->world()["proto"] = proto_ptr;
                conn_ptr->world()["trans"] = transport.get();
            }

            void on_disconnect(connection_ptr& conn_ptr, const cytx::net_result& err) override
            {
                log_->debug("conn {} disconnect", conn_ptr->get_conn_id());

                tprotocol_t* proto_ptr = conn_ptr->world()["proto"];
                if (proto_ptr != nullptr)
                {
                    delete proto_ptr;
                    proto_ptr = nullptr;
                    conn_ptr->world()["proto"] = proto_ptr;
                }
            }

            void on_receive(connection_ptr& conn_ptr, const msg_ptr& msgp) override
            {
                tprotocol_t* proto_ptr = conn_ptr->world()["proto"];
                thrift_transport* trans = conn_ptr->world()["trans"];
                if (!proto_ptr || !trans)
                {
                    log_->error("conn {} invalid, no proto or trans", conn_ptr->get_conn_id());
                    return;
                }

                tprotocol_ptr proto(proto_ptr, [](auto ptr) {});
                trans->set_current_msg(msgp);
                if (!processer_->process(proto, proto, nullptr))
                {
                    log_->warn("invalid msg, will close conn");
                    conn_ptr->close();
                }
            }

        protected:
            void init(const thrift_server_options& options)
            {
                net::server_options spotions;
                spotions.batch_send_msg = true;
                spotions.disconnect_interval = options.disconnect_interval;
                spotions.thread_mode = options.thread_mode;
                processer_ = options.processer;
                server_ = std::make_unique<server_t>(options.ip, options.port, spotions, this);

                log_ = cytx::log::force_get_log("thrift");
            }

        protected:
            server_ptr server_;
            processer_ptr processer_;
            log_ptr_t log_;
        };
    }
}