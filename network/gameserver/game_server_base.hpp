﻿#pragma once
#include "network/util/net.hpp"
#include "network/util/cast.hpp"
#include "network/meta/meta.hpp"
#include "network/net/tcp_server.hpp"
#include "network/service/service_manager.hpp"
#include "server_config.hpp"
#include "msg_pack.hpp"
#include "protocol.hpp"
#include "config_service.hpp"

namespace cytx
{
    namespace gameserver
    {
        namespace detail
        {
            using namespace cytx;
            using namespace cytx::gameserver;

            using msg_t = net::server_msg<net::msg_body>;
            using header_t = typename msg_t::header_t;
            using server_t = net::tcp_server<msg_t>;
            using server_ptr = std::unique_ptr<server_t>;
            using connection_t = typename server_t::connection_t;
            using connection_ptr = typename server_t::connection_ptr;
            using msg_ptr = typename server_t::msg_ptr;
            using irouter_t = typename server_t::irouter_t;
            using irouter_ptr = typename server_t::irouter_ptr;
            using timer_manager_t = timer_manager;
            using timer_manager_ptr = std::unique_ptr<timer_manager_t>;
            using timer_t = timer_proxy;
            using timer_func_t = std::function<bool()>;
            using context_t = typename connection_t::context_t;

            using before_invoke_func_t = std::function<void(const header_t&)>;
            using before_send_func_t = std::function<void(const header_t&)>;

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
                    world_["game_server"] = this;
                    world_["svc_manager"] = &service_mgr_;

                    //初始化内存池
                    memory_pool::ins().init();

                    //读取配置文件
                    config_mgr_.init(config_file_name);
                    config_mgr_.parse_real_ip(cytx::util::domain2ip);

                    //初始化日志
                    const server_info& info = config_mgr_[unique_id_];
                    cytx::log::get().init(fmt::format("logs/{}", info.name), info.log_level);

                    for (auto& log_cfg : info.logs)
                    {
                        auto log_ptr = log::init_log(fmt::format("logs/{}", log_cfg.filename), log_cfg.level, log_cfg.name, log_cfg.console);
                        logs_.push_back(log_ptr);
                    }

                    log_ = cytx::log::force_get_log("server");

                    log_->debug("init server");
                    //初始化server和timer
                    net::server_options options;
                    options.thread_mode = (net::server_thread_mode)info.thread_mode;
                    options.disconnect_interval = info.disconnect_heartbeat;
                    options.batch_send_msg = info.batch_send_msg;

                    server_ = std::make_unique<server_t>(info.ip, info.port, options, this);
                    timer_mgr_ = std::make_unique<timer_manager_t>(server_->get_io_service(), cytx::log::get_log("timer"));
                    flush_log_timer_ = timer_mgr_->set_auto_timer(info.flush_log_time * 1000, cytx::bind(&this_t::flush_logs, this));

                    log_->debug("begin register service");
                    //注册所有的service
                    service_mgr_.reg_inter_service(new config_service(config_mgr_), "config");
                    if (info.services)
                    {
                        log_->debug("register {} service", info.services->size());
                        service_mgr_.register_service(info.services.get());
                    }
                    else
                    {
                        log_->debug("register all service");
                        service_mgr_.register_all_service();
                    }
                }

                void start()
                {
                    log_->debug("start service");
                    service_mgr_.start_service();

                    log_->debug("start server");
                    flush_log_timer_.start();
                    server_->start();
                }
                void stop()
                {
                    log_->debug("stop server");
                    timer_mgr_->stop_all_timer();
                    server_->stop();

                    log_->debug("stop service");
                    service_mgr_.stop_service();

                    flush_logs();
                }

            public:
                // service
                template<typename T>
                T* get_service() const
                {
                    return service_mgr_.get_service<T>();
                }

                template<typename T>
                bool find_service() const
                {
                    return service_mgr_.find_service<T>();
                }

                bool find_service(const std::string& service_name) const
                {
                    return service_mgr_.find_service(service_name);
                }

                void reset_services()
                {
                    service_mgr_.reset_service();
                }

            public:
                //timer
                timer_t set_timer(int milliseconds, timer_func_t func)
                {
                    return timer_mgr_->set_timer(milliseconds, func);
                }

                timer_t set_fix_timer(int milliseconds, timer_func_t func)
                {
                    return timer_mgr_->set_fix_timer(milliseconds, func);
                }

                timer_t set_fix_timer(int milliseconds, std::function<void()> func)
                {
                    return timer_mgr_->set_fix_timer(milliseconds, func);
                }

                timer_t set_once_timer(int milliseconds, std::function<void()> func)
                {
                    return timer_mgr_->set_once_timer(milliseconds, func);
                }

                timer_t set_auto_timer(int milliseconds, std::function<void()> func)
                {
                    return timer_mgr_->set_auto_timer(milliseconds, func);
                }

            public:
                void add_before_invoke_func(before_invoke_func_t&& func)
                {
                    if (func)
                    {
                        before_invoke_funcs_.emplace_back(func);
                    }
                }
                void add_before_send_func(before_send_func_t&& func)
                {
                    if (func)
                    {
                        before_send_funcs_.emplace_back(func);
                    }
                }
            public:
                //给服务端发送消息
                template<typename MSG_ID, typename T>
                uint32_t send_server_msg(server_unique_id unique_id, MSG_ID msg_id, const T& t, const msg_ptr& request_msgp = nullptr)
                {
                    connection_ptr conn_ptr = get_connection_ptr(unique_id);
                    if (!conn_ptr)
                    {
                        LOG_WARN("server {} is not connected", unique_id);
                        return 0;
                    }

                    msg_ptr msgp = server_pack_msg(msg_id, unique_id, t);
                    if (request_msgp)
                    {
                        msgp->header().call_id = request_msgp->header().call_id;
                    }
                    send_raw_msg(conn_ptr, msgp);
                    return msgp->total_length();
                }

                template<typename MSG_ID, typename T>
                uint32_t send_client_msg(int32_t user_id, MSG_ID msg_id, const T& t)
                {
                    connection_ptr conn_ptr = get_connection_ptr(server_unique_id::gateway_server);
                    if (!conn_ptr)
                    {
                        LOG_WARN("gateway is not connected");
                        return 0;
                    }

                    msg_ptr msgp = server_pack_msg(msg_id, server_unique_id::gateway_server, t);
                    msgp->header().user_id = user_id;

                    send_raw_msg(conn_ptr, msgp);
                    return msgp->total_length();
                }

                template<typename MSG_ID, typename T>
                uint32_t broadcast_client_msg(const std::vector<int32_t>& users, MSG_ID msg_id, const T& t)
                {
                    connection_ptr conn_ptr = get_connection_ptr(server_unique_id::gateway_server);
                    if (!conn_ptr)
                    {
                        LOG_WARN("gateway is not connected");
                        return 0;
                    }

                    msg_ptr msgp = server_pack_msg(BroadcastMsg, server_unique_id::gateway_server, (uint32_t)msg_id, users, t);
                    send_raw_msg(conn_ptr, msgp);
                    return msgp->total_length();
                }

            public:
                // custom msg
                template<typename T>
                uint32_t send_server_msg(server_unique_id unique_id, const T& t, const msg_ptr& request_msgp = nullptr)
                {
                    connection_ptr conn_ptr = get_connection_ptr(unique_id);
                    if (!conn_ptr)
                    {
                        LOG_WARN("server {} is not connected", unique_id);
                        return 0;
                    }

                    msg_ptr msgp = server_pack_msg(unique_id, t);
                    if (request_msgp)
                    {
                        msgp->header().call_id = request_msgp->header().call_id;
                    }
                    send_raw_msg(conn_ptr, msgp);
                    return msgp->total_length();
                }

                template<typename T>
                uint32_t send_client_msg(int32_t user_id, const T& t)
                {
                    connection_ptr conn_ptr = get_connection_ptr(server_unique_id::gateway_server);
                    if (!conn_ptr)
                    {
                        LOG_WARN("gateway is not connected");
                        return 0;
                    }

                    msg_ptr msgp = server_pack_msg(server_unique_id::gateway_server, t);
                    msgp->header().user_id = user_id;

                    send_raw_msg(conn_ptr, msgp);
                    return msgp->total_length();
                }

                template<typename T>
                uint32_t broadcast_client_msg(const std::vector<int32_t>& users, const T& t)
                {
                    connection_ptr conn_ptr = get_connection_ptr(server_unique_id::gateway_server);
                    if (!conn_ptr)
                    {
                        LOG_WARN("gateway is not connected");
                        return 0;
                    }

                    msg_ptr msgp = server_pack_msg(BroadcastMsg, server_unique_id::gateway_server, t.get_protocol_id(), users, t);
                    send_raw_msg(conn_ptr, msgp);
                    return msgp->total_length();
                }

                template<typename RETURN_T, typename T>
                RETURN_T await_server_msg(server_unique_id unique_id, const T& t, context_t ctx)
                {
                    RETURN_T ret{};

                    connection_ptr conn_ptr = get_connection_ptr(unique_id);
                    if (!conn_ptr)
                    {
                        LOG_WARN("server {} is not connected", unique_id);
                        return ret;
                    }

                    msg_ptr send_msgp = server_pack_msg(unique_id, t);
                    msg_ptr msgp = conn_ptr->await_write(send_msgp, ctx);
                    ret = unpack_msg<RETURN_T>(msgp);
                    return ret;
                }

            public:
                //给服务端发送消息
                uint32_t send_server_msg(server_unique_id unique_id, msg_ptr& msgp, const msg_ptr& request_msgp = nullptr)
                {
                    connection_ptr conn_ptr = get_connection_ptr(unique_id);
                    if (!conn_ptr)
                    {
                        LOG_WARN("server {} is not connected", unique_id);
                        return 0;
                    }

                    msgp->header().from_unique_id = (uint16_t)this->unique_id_;
                    msgp->header().to_unique_id = (uint16_t)unique_id;

                    if (request_msgp)
                    {
                        msgp->header().call_id = request_msgp->header().call_id;
                    }
                    send_raw_msg(conn_ptr, msgp);
                    return msgp->total_length();
                }

                uint32_t send_client_msg(int32_t user_id, msg_ptr& msgp)
                {
                    connection_ptr conn_ptr = get_connection_ptr(server_unique_id::gateway_server);
                    if (!conn_ptr)
                    {
                        LOG_WARN("gateway is not connected");
                        return 0;
                    }

                    msgp->header().user_id = user_id;

                    send_raw_msg(conn_ptr, msgp);
                    return msgp->total_length();
                }

                uint32_t broadcast_client_msg(const std::vector<int32_t>& users, msg_ptr& msgp)
                {
                    connection_ptr conn_ptr = get_connection_ptr(server_unique_id::gateway_server);
                    if (!conn_ptr)
                    {
                        LOG_WARN("gateway is not connected");
                        return 0;
                    }

                    msg_ptr new_msgp = server_pack_msg(BroadcastMsg, server_unique_id::gateway_server, msgp->header().protocol_id, users, msgp);
                    send_raw_msg(conn_ptr, new_msgp);
                    return new_msgp->total_length();
                }
            protected:
                void on_connect(connection_ptr& conn_ptr, const net_result& err) override
                {
                    LOG_DEBUG("new conenct {}, {}", conn_ptr->get_conn_id(), err.message());
                }
                void on_disconnect(connection_ptr& conn_ptr, const net_result& err) override
                {
                    if (conn_ptr == gate_conn_ptr_)
                    {
                        LOG_ERROR("gate way connect disconnected, {}", err.message());
                        gate_conn_ptr_ = nullptr;
                    }
                    else
                    {
                        LOG_DEBUG("conn {} disconnect {}", conn_ptr->get_conn_id(), err.message());
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
                            gate_conn_ptr_->world()["unique_id"] = unique_id;
                            LOG_INFO("gate way connected");
                        }
                        else
                        {
                            LOG_INFO("connect register, server is {}", unique_id);
                        }
                    }
                    else if(!handle_msg(conn_ptr, msgp))
                    {
                        LOG_DEBUG("receive msg {}", protocol_id);
                    }
                }
                void on_receive_msgs(connection_ptr& conn_ptr, const std::vector<msg_ptr>& msg_list) override
                {
                    for (auto& msgp : msg_list)
                    {
                        auto& header = msgp->header();
                        for (auto& func : before_invoke_funcs_)
                        {
                            func(header);
                        }
                        this->on_receive(conn_ptr, msgp);
                    }
                }

                virtual connection_ptr get_connection_ptr(server_unique_id unique_id) const
                {
                    if (unique_id == server_unique_id::gateway_server)
                        return get_running_conn(gate_conn_ptr_);
                    else
                        return nullptr;
                }

                virtual bool handle_msg(connection_ptr& conn_ptr, const msg_ptr& msgp)
                {
                    return false;
                }
            protected:
                template<typename MSG_ID, typename T>
                uint32_t send_server_msg(connection_ptr& server_conn_ptr, MSG_ID msg_id, const T& t, const msg_ptr& request_msgp = nullptr)
                {
                    connection_ptr conn_ptr = get_running_conn(server_conn_ptr);
                    if (!conn_ptr)
                    {
                        LOG_WARN("the server is not connected, sending msg {}", (uint32_t)msg_id);
                        return 0;
                    }

                    server_unique_id unique_id = conn_ptr->world()["unique_id"];
                    msg_ptr msgp = server_pack_msg(msg_id, unique_id, t);
                    if (request_msgp)
                    {
                        msgp->header().call_id = request_msgp->header().call_id;
                    }
                    send_raw_msg(conn_ptr, msgp);
                    return msgp->total_length();
                }

                void send_raw_msg(connection_ptr& conn_ptr, msg_ptr& msgp)
                {
                    auto& header = msgp->header();
                    for (auto& func : before_send_funcs_)
                    {
                        func(header);
                    }
                    conn_ptr->write(msgp);
                }

                template<typename MSG_ID, typename ... ARGS>
                msg_ptr server_pack_msg(MSG_ID msg_id, server_unique_id to_unique_id, const ARGS& ... args)
                {
                    msg_ptr msgp = pack_msg(args ...);
                    auto& header = msgp->header();
                    header.protocol_id = (uint32_t)msg_id;
                    header.from_unique_id = (uint16_t)unique_id_;
                    header.to_unique_id = (uint16_t)to_unique_id;
                    return msgp;
                }

                template<typename T>
                msg_ptr server_pack_msg(server_unique_id to_unique_id, const T& t)
                {
                    return server_pack_msg(t.get_protocol_id(), to_unique_id, t);
                }

                connection_ptr get_running_conn(const connection_ptr& conn) const
                {
                    return conn && conn->is_running() ? conn : nullptr;
                }

            protected:
                void flush_logs()
                {
                    cytx::log::get().flush();

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

                world_map world_;
                service_manager service_mgr_;

                std::vector<before_invoke_func_t> before_invoke_funcs_;
                std::vector<before_send_func_t> before_send_funcs_;
                log_ptr_t log_;
            };
        }

        using game_server_base = detail::game_server_base;
    }
}