#pragma once
#include "game_server_base.hpp"
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

                    depend_db_ = unique_id_ != server_unique_id::db_server;

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

                    //连接各个服务
                    const server_info& info = config_mgr_[server_unique_id::center_server];
                    connect_center_timer_ = timer_mgr_->set_auto_timer(info.connect_interval, connect_func);
                    connect_center_timer_.invoke();
                    connect_center_timer_.start();

                    if (depend_db_)
                    {
                        db_conn_ptr_ = server_->create_connection();
                        connect_db_timer_ = timer_mgr_->set_auto_timer(db_info_.connect_interval, connect_db_func);
                    }
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
                    /*send_server_msg(server_unique_id::center_server, CS_RegisterServer, send_data);*/

                    /*SCServerEvent data = async_await_msg<SCServerEvent>(server_unique_id::center_server, CS_RegisterServer, send_data);

                    LOG_DEBUG("server event {}, {}", (uint32_t)data.event, (uint16_t)data.info.unique_id);*/

                    async_await_msg<SCServerEvent>(server_unique_id::center_server, CS_RegisterServer, send_data, [](SCServerEvent& data)
                    {
                        LOG_DEBUG("server event {}, {}", (uint32_t)data.event, (uint16_t)data.info.unique_id);
                    });
                }
                void get_server_info()
                {
                    CSGetServerInfo send_data;
                    send_data.servers.push_back(server_unique_id::db_server);
                    send_server_msg(server_unique_id::center_server, CS_GetServerInfo, send_data);
                }
                void on_sc_register_server(const msg_ptr& msgp)
                {
                    SCRegisterServer data = unpack_msg<SCRegisterServer>(msgp);
                    LOG_INFO("current server uid {}", data.server_uid);
                }
                void on_sc_get_server_info(const msg_ptr& msgp)
                {
                    SCGetServerInfo data = unpack_msg<SCGetServerInfo>(msgp);
                    if (depend_db_)
                    {
                        auto it = data.servers.find(server_unique_id::db_server);
                        if (it != data.servers.end())
                        {
                            db_info_ = it->second;
                            connect_db_timer_.invoke();
                            connect_db_timer_.start();
                        }
                    }
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
                        get_server_info();
                    }
                    else
                    {
                        base_t::on_disconnect(conn_ptr, err);
                    }
                }
                void on_receive(connection_ptr& conn_ptr, const msg_ptr& msgp) override
                {
                    uint32_t protocol_id = msgp->header().protocol_id;
                    if (protocol_id == SC_RegisterServer)
                    {
                        on_sc_register_server(msgp);
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
                    return base_t::get_connection_ptr(unique_id);
                }

            protected:
                bool depend_db_ = true;
                connection_ptr center_conn_ptr_;
                connection_ptr db_conn_ptr_;
                timer_t connect_center_timer_;
                timer_t connect_db_timer_;
                ServerInfo db_info_;
            };
        }
        using game_server = detail::game_server;
    }
}