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

                    center_conn_ptr_ = server_->create_connection();
                    auto connect_func = [this]
                    {
                        const server_info& info = config_mgr_[server_unique_id::center_server];
                        center_conn_ptr_->async_connect(info.ip, info.port);
                    };

                    //连接各个服务
                    const server_info& info = config_mgr_[server_unique_id::center_server];
                    connect_center_timer_ = timer_mgr_->set_auto_timer(info.connect_interval, connect_func);
                    connect_center_timer_.invoke();
                    connect_center_timer_.start();
                }

                template<typename MSG_ID, typename T>
                void broadcast_server_msg(const std::vector<server_unique_id>& servers, MSG_ID msg_id, const T& t)
                {
                    connection_ptr conn_ptr = get_connection_ptr(server_unique_id::center_server);
                    if (!conn_ptr)
                    {
                        //TODO log error
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
                void on_register_server(const msg_ptr& msgp)
                {
                    SCRegisterServer data = unpack_msg<SCRegisterServer>(msgp);
                    LOG_INFO("current server uid {}", data.server_uid);
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
                            register_to_center();
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
                        on_register_server(msgp);
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

                    return base_t::get_connection_ptr(unique_id);
                }

            protected:
                connection_ptr center_conn_ptr_;
                timer_t connect_center_timer_;
            };
        }
        using game_server = detail::game_server;
    }
}