#pragma once
#include "../base/utils.hpp"
#include "server.hpp"
#include "server_config.hpp"
#include "msg_pack.hpp"

namespace cytx
{
    namespace gameserver
    {
        namespace detail
        {
            using namespace cytx;
            using namespace cytx::gameserver;

            using msg_t = cytx::gameserver::server_msg<cytx::gameserver::msg_body>;
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

            class game_server : public irouter_t
            {
                using this_t = game_server;
            public:
                game_server(server_unique_id unique_id)
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

                    //连接各个服务
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
                void send_msg(server_unique_id unique_id, MSG_ID msg_id, const T& t)
                {
                    inter_send_msg(unique_id, msg_id, t);
                }

                template<typename MSG_ID, typename T>
                void send_client_msg(int32_t user_id, MSG_ID msg_id, const T& t)
                {
                    connection_ptr conn_ptr = gate_conn_ptr_;
                    if (!conn_ptr)
                    {
                        //TODO log error
                        return;
                    }

                    msg_ptr msgp = pack_msg(msg_id, server_unique_id::gateway_server, t);
                    msgp->header().user_id = user_id;

                    send_raw_msg(conn_ptr, msgp);
                }

                template<typename MSG_ID, typename T>
                void broadcast_client_msg(const std::vector<int32_t>& users, MSG_ID msg_id, const T& t)
                {
                    const uint32_t broadcast_msg_id = 0x60a001a;

                    connection_ptr conn_ptr = gate_conn_ptr_;
                    if (!conn_ptr)
                    {
                        //TODO log error
                        return;
                    }

                    msg_ptr msgp = pack_msg(broadcast_msg_id, server_unique_id::gateway_server, (uint32_t)msg_id, users, t);
                    send_raw_msg(conn_ptr, msgp);
                }

            public:
                template<typename RETURN_T, typename MSG_ID, typename T>
                RETURN_T async_await_msg(server_unique_id unique_id, MSG_ID msg_id, const T& t)
                {
                    connection_ptr conn_ptr = get_connection_ptr(unique_id);
                    if (!conn_ptr)
                    {
                        //TODO log error
                        return nullptr;
                    }

                    msg_ptr send_msgp = pack_msg(msg_id, unique_id, t);
                    msg_ptr msgp = conn_ptr->await_write(send_msgp);
                    return unpack_msg<RETURN_T>(msgp);
                }
            private:
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

                    const uint32_t gw_register_server = 0x40a0009;
                    if (protocol_id == gw_register_server && gate_conn_ptr_ == nullptr)
                    {
                        auto unique_id = unpack_msg<server_unique_id>(msgp);
                        if (unique_id == server_unique_id::gateway_server)
                        {
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

                template<typename MSG_ID, typename T>
                msg_ptr inter_send_msg(server_unique_id unique_id, MSG_ID msg_id, const T& t)
                {
                    connection_ptr conn_ptr = get_connection_ptr(unique_id);
                    if (!conn_ptr)
                    {
                        //TODO log error
                        return nullptr;
                    }

                    msg_ptr msgp = pack_msg(msg_id, unique_id, t);
                     send_raw_msg(conn_ptr, msgp);
                    return msgp;
                }

                void send_raw_msg(connection_ptr& conn_ptr, msg_ptr& msgp)
                {
                    conn_ptr->write(msgp);
                }

                connection_ptr get_connection_ptr(server_unique_id unique_id) const
                {
                    if (unique_id == server_unique_id::gateway_server)
                        return gate_conn_ptr_;
                    else if (unique_id == server_unique_id::db_server)
                        return db_conn_ptr_;
                    else
                        return center_conn_ptr_;
                }

                template<typename MSG_ID, typename ... ARGS>
                msg_ptr pack_msg(MSG_ID msg_id, server_unique_id to_unique_id, const ARGS& ... args)
                {
                    msg_ptr msgp = pack_msg(header_t::big_endian(), args ...);
                    auto& header = msgp->header();
                    header.protocol_id = (uint32_t)msg_id;
                    header.from_unique_id = (uint16_t)unique_id_;
                    header.to_unique_id = (uint16_t)to_unique_id;
                    return msgp;
                }

            private:
                void flush_logs()
                {
                    for (auto& log_ptr : logs_)
                    {
                        log_ptr->flush();
                    }
                }
            private:
                server_unique_id unique_id_;
                server_config_manager config_mgr_;
                server_ptr server_;
                timer_manager_ptr timer_mgr_;
                //call_manager_ptr call_mgr_;

                std::vector<log_ptr_t> logs_;
                timer_t flush_log_timer_;

                connection_ptr gate_conn_ptr_;
                connection_ptr db_conn_ptr_;
                connection_ptr center_conn_ptr_;
                //center是主动连接
                //DB是主动连接
                //其他服务不确定
            };
        }
        using game_server = detail::game_server;
    }
}