#pragma once
#include "game_server_base.hpp"
#include "proto.hpp"

namespace cytx
{
    namespace gameserver
    {
        namespace detail
        {
            class game_server : public game_server_base
            {
                using this_t = game_server;
                using base_t = game_server_base;
            public:
                game_server(server_unique_id unique_id)
                    : base_t(unique_id)
                {

                }

                void init(const std::string& config_file_name = "server_config.xml")
                {
                    base_t::init(config_file_name);

                    const server_info& info = config_mgr_[unique_id_];
                    depend_db_ = unique_id_ != server_unique_id::db_server;
                    if (depend_db_)
                    {
                        auto it = std::find(info.depends.begin(), info.depends.end(), server_unique_id::db_server);
                        depend_db_ = it != info.depends.end();
                    }

                    center_conn_ptr_ = server_->create_connection();

                    auto connect_func = [this]
                    {
                        const server_info& info = config_mgr_[server_unique_id::center_server];
                        center_conn_ptr_->async_connect(info.ip, info.port);
                    };
                    auto connect_db_func = [this]
                    {
                        db_conn_ptr_->async_connect(db_info_.ip, db_info_.port);
                    };
                    auto get_db_info_func = [this]
                    {
                        if (db_conn_ptr_->is_running())
                            return;

                        connect_db_timer_.stop();

                        get_server_info();
                    };

                    //连接各个服务
                    const server_info& center_info = config_mgr_[server_unique_id::center_server];
                    connect_center_timer_ = timer_mgr_->set_auto_timer(center_info.connect_interval, connect_func);
                    connect_center_timer_.start(true);

                    if (depend_db_)
                    {
                        db_conn_ptr_ = server_->create_connection();
                        connect_db_timer_ = timer_mgr_->set_auto_timer(db_info_.connect_interval, connect_db_func);
                        get_db_info_timer_ = timer_mgr_->set_auto_timer(20 * 1000, get_db_info_func);
                    }

                    //初始化所有的service
                    service_mgr_.init_service();
                }

                template<typename MSG_ID, typename T>
                void broadcast_server_msg(const std::vector<server_unique_id>& servers, MSG_ID msg_id, const T& t)
                {
                    connection_ptr conn_ptr = get_connection_ptr(server_unique_id::center_server);
                    if (!conn_ptr)
                    {
                        LOG_WARN("center server is not connected");
                        return;
                    }

                    msg_ptr msgp = server_pack_msg(BroadcastServerMsg, server_unique_id::center_server, (uint32_t)msg_id, servers, t);
                    send_raw_msg(conn_ptr, msgp);
                }

            protected:
                void register_to_center()
                {
                    const server_info& info = config_mgr_[unique_id_];

                    CSRegisterServer send_data{ unique_id_, info.ip, info.port };
                    send_server_msg(server_unique_id::center_server, CS_RegisterServer, send_data);
                }
                void get_server_info()
                {
                    if (depend_db_)
                    {
                        LOG_DEBUG("try get db info");
                        CSGetServerInfo send_data;
                        send_data.servers.push_back(server_unique_id::db_server);
                        send_server_msg(server_unique_id::center_server, CS_GetServerInfo, send_data);
                    }
                }
                void on_sc_register_server(const msg_ptr& msgp)
                {
                    SCRegisterServer data = unpack_msg<SCRegisterServer>(msgp);
                    LOG_INFO("current server uid {}", data.server_uid);

                    reconnect_db();
                }
                void on_sc_get_server_info(const msg_ptr& msgp)
                {
                    SCGetServerInfo data = unpack_msg<SCGetServerInfo>(msgp);
                    if (depend_db_)
                    {
                        auto it = data.servers.find(server_unique_id::db_server);
                        if (it != data.servers.end())
                        {
                            LOG_DEBUG("geted db info, {}:{}", it->second.ip, it->second.port);
                            on_get_db_info(it->second);
                        }
                        else
                        {
                            LOG_DEBUG("on sc_get_server_info, don't have db info");
                        }
                    }
                }
                void on_sc_server_event(const msg_ptr& msgp)
                {
                    SCServerEvent data = unpack_msg<SCServerEvent>(msgp);
                    if (data.event == EServerConnected)
                    {
                        LOG_DEBUG("server {} connected", (uint16_t)data.info.unique_id);
                        if (data.info.unique_id == server_unique_id::db_server)
                        {
                            on_get_db_info(data.info);
                        }
                    }
                    else if (data.event == EServerDisconnected)
                    {
                        LOG_DEBUG("server {} disconnected", (uint16_t)data.info.unique_id);
                    }
                    else
                    {
                        LOG_DEBUG("server {} event {}", (uint16_t)data.info.unique_id, (uint32_t)data.event);
                    }
                }

                void reconnect_db()
                {
                    if (!depend_db_)
                        return;

                    get_db_info_timer_.start(true);
                }
                void on_get_db_info(const ServerInfo& db_info)
                {
                    db_info_ = db_info;
                    connect_db_timer_.start(true);
                }
            protected:
                void on_connect(connection_ptr& conn_ptr, const net_result& err) override
                {
                    if (conn_ptr == center_conn_ptr_)
                    {
                        if (err)
                        {
                            LOG_WARN("connect center server failed, {}", err.message());
                        }
                        else
                        {
                            LOG_DEBUG("connect center server success");
                            connect_center_timer_.stop();
                            center_conn_ptr_->start();
                            register_to_center();
                        }
                    }
                    else if (depend_db_ && conn_ptr == db_conn_ptr_)
                    {
                        if (err)
                        {
                            LOG_WARN("connect db server failed, {}", err.message());
                        }
                        else
                        {
                            LOG_DEBUG("connect db server success");
                            connect_db_timer_.stop();
                            get_db_info_timer_.stop();
                            db_conn_ptr_->start();
                        }
                    }
                    else
                    {
                        base_t::on_connect(conn_ptr, err);
                    }
                }
                void on_disconnect(connection_ptr& conn_ptr, const net_result& err) override
                {
                    if (conn_ptr == center_conn_ptr_)
                    {
                        LOG_ERROR("center connect disconnected, {}", err.message());
                        connect_center_timer_.start();
                    }
                    else if (depend_db_ && conn_ptr == db_conn_ptr_)
                    {
                        LOG_ERROR("db connect disconnected, {}", err.message());
                        reconnect_db();
                    }
                    else
                    {
                        base_t::on_disconnect(conn_ptr, err);
                    }
                }
                void on_receive(connection_ptr& conn_ptr, const msg_ptr& msgp) override
                {
                    msg_ptr msgptr = msgp;
                    proto_ptr_t proto = Proto::Decode(msgptr);
                    if (proto)
                    {
                        proto->process(msgptr, conn_ptr, *this);
                        return;
                    }

                    uint32_t protocol_id = msgp->header().protocol_id;
                    if (protocol_id == SC_RegisterServer)
                    {
                        on_sc_register_server(msgp);
                    }
                    else if (protocol_id == SC_GetServerInfo)
                    {
                        on_sc_get_server_info(msgp);
                    }
                    else if (protocol_id == SC_ServerEvent)
                    {
                        on_sc_server_event(msgp);
                    }
                    else
                    {
                        base_t::on_receive(conn_ptr, msgp);
                    }
                }

                connection_ptr get_connection_ptr(server_unique_id unique_id) const override
                {
                    if (unique_id == server_unique_id::center_server)
                        return get_running_conn(center_conn_ptr_);
                    else if (unique_id == server_unique_id::db_server)
                        return get_running_conn(db_conn_ptr_);
                    else if (unique_id != server_unique_id::gateway_server)
                        return get_running_conn(center_conn_ptr_);
                    return base_t::get_connection_ptr(unique_id);
                }

            protected:
                connection_ptr center_conn_ptr_;
                timer_t connect_center_timer_;

                bool depend_db_ = true;
                connection_ptr db_conn_ptr_;
                timer_t connect_db_timer_;
                timer_t get_db_info_timer_;
                ServerInfo db_info_;
            };
        }
        using game_server = detail::game_server;
    }
}