#pragma once
#include "../base/utils.hpp"
#include "tcp_server.hpp"
#include "server_config.hpp"
#include "msg_pack.hpp"
#include "protocol.hpp"

namespace cytx
{
    namespace gameserver
    {
        namespace detail
        {
            using namespace cytx;
            using namespace cytx::gameserver;

            using msg_t = server_msg<msg_body>;
            using header_t = typename msg_t::header_t;
            using server_t = tcp_server<msg_t>;
            using server_ptr = std::unique_ptr<server_t>;
            using connection_t = typename server_t::connection_t;
            using connection_ptr = typename server_t::connection_ptr;
            using msg_ptr = typename server_t::msg_ptr;
            using irouter_t = typename server_t::irouter_t;
            using irouter_ptr = typename server_t::irouter_ptr;
            using timer_manager_t = timer_manager;
            using timer_manager_ptr = std::unique_ptr<timer_manager_t>;
            using timer_t = timer_proxy;

            class game_server_base : public irouter_t
            {
                using this_t = game_server_base;
            public:
                game_server_base(server_unique_id unique_id)
                    : unique_id_(unique_id)
                {

                }
                void init(const std::string& config_file_name = "server_config.xml")
                {
                    //初始化内存池
                    MemoryPoolManager::get_mutable_instance().init();

                    cytx::log::get().init(log_level_t::debug);
                    //读取配置文件
                    config_mgr_.init(config_file_name);
                    config_mgr_.parse_real_ip(cytx::util::domain2ip);

                    //初始化日志
                    const server_info& info = config_mgr_[unique_id_];
                    for (auto& log_cfg : info.logs)
                    {
                        auto log_ptr = log::init_log(fmt::format("logs/{}", log_cfg.filename), log_cfg.level, log_cfg.name, log_cfg.console);
                        logs_.push_back(log_ptr);
                    }

                    //初始化server和timer
                    server_options options;
                    options.thread_mode = (cytx::gameserver::server_thread_mode)info.thread_mode;
                    options.disconnect_interval = info.disconnect_heartbeat;
                    options.batch_send_msg = info.batch_send_msg;

                    server_ = std::make_unique<server_t>(info.ip, info.port, options, this);
                    timer_mgr_ = std::make_unique<timer_manager_t>(server_->get_io_service());
                    flush_log_timer_ = timer_mgr_->set_auto_timer(info.flush_log_time, cytx::bind(&this_t::flush_logs, this));
                }

                void start()
                {
                    flush_log_timer_.start();
                    server_->start();
                }
                void stop()
                {
                    timer_mgr_->stop_all_timer();
                    server_->stop();

                    flush_logs();
                }

            public:
                //给服务端发送消息
                template<typename MSG_ID, typename T>
                void send_server_msg(server_unique_id unique_id, MSG_ID msg_id, const T& t, const msg_ptr& request_msgp = nullptr)
                {
                    connection_ptr conn_ptr = get_connection_ptr(unique_id);
                    if (!conn_ptr)
                    {
                        LOG_WARN("server {} is not connected", (uint16_t)unique_id);
                        return;
                    }

                    msg_ptr msgp = server_pack_msg(msg_id, unique_id, t);
                    if (request_msgp)
                    {
                        msgp->header().call_id = request_msgp->header().call_id;
                    }
                    send_raw_msg(conn_ptr, msgp);
                }

                template<typename MSG_ID, typename T>
                void send_server_msg(connection_ptr& server_conn_ptr, MSG_ID msg_id, const T& t, const msg_ptr& request_msgp = nullptr)
                {
                    connection_ptr conn_ptr = get_running_conn(server_conn_ptr);
                    if (!conn_ptr)
                    {
                        LOG_WARN("the server is not connected");
                        return;
                    }

                    server_unique_id unique_id = conn_ptr->get_conn_info().unique_id;
                    msg_ptr msgp = server_pack_msg(msg_id, unique_id, t);
                    if (request_msgp)
                    {
                        msgp->header().call_id = request_msgp->header().call_id;
                    }
                    send_raw_msg(conn_ptr, msgp);
                }

                template<typename MSG_ID, typename T>
                void send_client_msg(int32_t user_id, MSG_ID msg_id, const T& t)
                {
                    connection_ptr conn_ptr = get_connection_ptr(server_unique_id::gateway_server);
                    if (!conn_ptr)
                    {
                        LOG_WARN("gateway is not connected");
                        return;
                    }

                    msg_ptr msgp = server_pack_msg(msg_id, server_unique_id::gateway_server, t);
                    msgp->header().user_id = user_id;

                    send_raw_msg(conn_ptr, msgp);
                }

                template<typename MSG_ID, typename T>
                void broadcast_client_msg(const std::vector<int32_t>& users, MSG_ID msg_id, const T& t)
                {
                    connection_ptr conn_ptr = get_connection_ptr(server_unique_id::gateway_server);
                    if (!conn_ptr)
                    {
                        LOG_WARN("server {} is not connected", (uint16_t)unique_id);
                        return;
                    }

                    msg_ptr msgp = server_pack_msg(BroadcastMsg, server_unique_id::gateway_server, (uint32_t)msg_id, users, t);
                    send_raw_msg(conn_ptr, msgp);
                }

            public:
                template<typename RETURN_T, typename MSG_ID, typename T>
                RETURN_T async_await_msg1(server_unique_id unique_id, MSG_ID msg_id, const T& t, const msg_ptr& request_msgp = nullptr)
                {
                    RETURN_T ret{};

                    connection_ptr conn_ptr = get_connection_ptr(unique_id);
                    if (!conn_ptr)
                    {
                        LOG_WARN("server {} is not connected", (uint16_t)unique_id);
                        return ret;
                    }

                    msg_ptr send_msgp = server_pack_msg(msg_id, unique_id, t);
                    if (request_msgp)
                    {
                        send_msgp->header().call_id = request_msgp->header().call_id;
                    }
                    msg_ptr msgp = conn_ptr->await_write(send_msgp);
                    ret = unpack_msg<RETURN_T>(msgp);
                    return ret;
                }

                template<typename RETURN_T, typename MSG_ID, typename T>
                void async_await_msg(server_unique_id unique_id, MSG_ID msg_id, const T& t, std::function<void(RETURN_T)>&& func, const msg_ptr& request_msgp = nullptr)
                {
                    connection_ptr conn_ptr = get_connection_ptr(unique_id);
                    if (!conn_ptr)
                    {
                        LOG_WARN("server {} is not connected", (uint16_t)unique_id);
                        return;
                    }

                    msg_ptr send_msgp = server_pack_msg(msg_id, unique_id, t);
                    if (request_msgp)
                    {
                        send_msgp->header().call_id = request_msgp->header().call_id;
                    }
                    conn_ptr->await_write(send_msgp, [this, f = std::forward<std::function<void(RETURN_T)>>(func)](msg_ptr& msgp)
                    {
                        RETURN_T ret = unpack_msg<RETURN_T>(msgp);
                        f(ret);
                    });
                }
            protected:
                void on_connect(connection_ptr& conn_ptr, const net_result& err) override
                {
                    LOG_DEBUG("new conenct {}", err.message());
                }
                void on_disconnect(connection_ptr& conn_ptr, const net_result& err) override
                {
                    if (conn_ptr == gate_conn_ptr_)
                    {
                        LOG_ERROR("gate way connect disconnected, {}", err.message());
                    }
                    else
                    {
                        LOG_DEBUG("new disconnect {}", err.message());
                    }
                }
                void on_receive(connection_ptr& conn_ptr, const msg_ptr& msgp) override
                {
                    uint32_t protocol_id = msgp->header().protocol_id;

                    if (protocol_id == SimpleRegisterServer && gate_conn_ptr_ == nullptr)
                    {
                        auto unique_id = unpack_msg<server_unique_id>(msgp);
                        if (unique_id == server_unique_id::gateway_server)
                        {
                            gate_conn_ptr_ = conn_ptr;
                            gate_conn_ptr_->set_conn_info(connection_info{ unique_id });
                            LOG_DEBUG("gate way connected");
                        }
                        else
                        {
                            LOG_DEBUG("connect register, server is {}", (uint32_t)unique_id);
                        }
                    }
                    else
                    {
                        LOG_DEBUG("receive msg {}", protocol_id);
                    }
                }

                virtual connection_ptr get_connection_ptr(server_unique_id unique_id) const
                {
                    if (unique_id == server_unique_id::gateway_server)
                        return get_running_conn(gate_conn_ptr_);
                    else
                        return nullptr;
                }
            protected:
                void send_raw_msg(connection_ptr& conn_ptr, msg_ptr& msgp)
                {
                    conn_ptr->write(msgp);
                }

                template<typename MSG_ID, typename ... ARGS>
                msg_ptr server_pack_msg(MSG_ID msg_id, server_unique_id to_unique_id, const ARGS& ... args)
                {
                    msg_ptr msgp = pack_msg(header_t::big_endian(), args ...);
                    auto& header = msgp->header();
                    header.protocol_id = (uint32_t)msg_id;
                    header.from_unique_id = (uint16_t)unique_id_;
                    header.to_unique_id = (uint16_t)to_unique_id;
                    return msgp;
                }

                connection_ptr get_running_conn(const connection_ptr& conn) const
                {
                    return conn && conn->is_running() ? conn : nullptr;
                }

            protected:
                void flush_logs()
                {
                    for (auto& log_ptr : logs_)
                    {
                        log_ptr->flush();
                    }
                }
            protected:
                server_unique_id unique_id_;
                server_config_manager config_mgr_;
                server_ptr server_;
                timer_manager_ptr timer_mgr_;

                std::vector<log_ptr_t> logs_;
                timer_t flush_log_timer_;

                connection_ptr gate_conn_ptr_;
            };
        }
    }
}