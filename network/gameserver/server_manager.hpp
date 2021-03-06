#pragma once
#include "game_server_base.hpp"
#include "protocol.hpp"

namespace cytx
{
    namespace gameserver
    {
        namespace detail
        {
            class server_manager : public game_server_base
            {
                struct manage_server_info
                {
                    connection_ptr conn_ptr;
                    ServerInfo info;
                };

                using this_t = server_manager;
                using base_t = game_server_base;
            public:
                server_manager(server_unique_id unique_id)
                    : base_t(unique_id)
                {

                }
                void init(const std::string& config_file_name = "server_config.xml")
                {
                    base_t::init(config_file_name);
                    log_->debug("init service");
                    service_mgr_.init_service();
                }

                template<typename MSG_ID, typename T>
                void broadcast_server_msg(const std::vector<server_unique_id>& servers, MSG_ID msg_id, const T& t)
                {
                    msg_ptr msgp = server_pack_msg(msg_id, unique_id_, t);
                    broadcast_server_msg(servers, msgp);
                }

                void broadcast_server_msg(const std::vector<server_unique_id>& servers, msg_ptr& msgp)
                {
                    for (auto unique_id : servers)
                    {
                        forward_msg(unique_id, msgp);
                    }
                }

            protected:
                void on_cs_register_server(connection_ptr& conn_ptr, const msg_ptr& msgp)
                {
                    CSRegisterServer data = unpack_msg<CSRegisterServer>(msgp);

                    manage_server_info server_info;
                    auto& info = server_info.info;
                    server_info.conn_ptr = conn_ptr;
                    info.unique_id = data.unique_id;
                    info.ip = data.ip;
                    info.port = data.port;
                    info.connect_interval = 5000;
                    servers_[data.unique_id].emplace_back(server_info);

                    conn_ptr->world()["unique_id"] = data.unique_id;
                    LOG_INFO("server {} register", data.unique_id);

                    send_server_msg(conn_ptr, SC_RegisterServer, SCRegisterServer{ (uint16_t)data.unique_id });

                    //通知其他服务
                    SCServerEvent server_event;
                    server_event.event = EServerConnected;
                    server_event.info = server_info.info;

                    broadcast_msg(data.unique_id, SC_ServerEvent, server_event);
                }

                void on_cs_get_server_info(connection_ptr& conn_ptr, const msg_ptr& msgp)
                {
                    CSGetServerInfo data = unpack_msg<CSGetServerInfo>(msgp);
                    SCGetServerInfo send_data;

                    for (server_unique_id unique_id : data.servers)
                    {
                        auto it = servers_.find(unique_id);
                        if(it == servers_.end() || it->second.empty())
                            continue;

                        auto& server_info = it->second.front().info;
                        send_data.servers.emplace(server_info.unique_id, server_info);
                    }

                    send_server_msg(conn_ptr, SC_GetServerInfo, send_data);
                }

                void on_broadcast_server_msg(connection_ptr& conn_ptr, const msg_ptr& msgp)
                {
                    BroadcastInfo data = unpack_msg<BroadcastInfo>(msgp);
                    if (data.servers.empty())
                        return;

                    //TODO 构造msgp
                    msg_ptr send_msgp = msgp;
                    broadcast_server_msg(data.servers, send_msgp);
                }

                void on_forward_msg(connection_ptr& conn_ptr, const msg_ptr& msgp)
                {
                    server_unique_id to_unique_id = (server_unique_id)(msgp->header().to_unique_id);
                    forward_msg(to_unique_id, msgp);
                }

            protected:
                void on_disconnect(connection_ptr& conn_ptr, const net_result& err) override
                {
                    server_unique_id unique_id = conn_ptr->world()["unique_id"];

                    auto it = servers_.find(unique_id);
                    if (it == servers_.end() || it->second.empty())
                    {
                        base_t::on_disconnect(conn_ptr, err);
                        return;
                    }

                    auto& server_info_list = it->second;
                    for (auto info_it = server_info_list.begin(); info_it != server_info_list.end(); ++info_it)
                    {
                        auto& info = *info_it;
                        if (info.conn_ptr == conn_ptr)
                        {
                            info.conn_ptr = nullptr;
                            LOG_ERROR("server {} disconnect", unique_id);
                            server_info_list.erase(info_it);
                            break;
                        }
                    }

                    //通知其他服务
                    SCServerEvent server_event;
                    server_event.event = EServerDisconnected;
                    server_event.info.unique_id = unique_id;
                    broadcast_msg(unique_id, SC_ServerEvent, server_event);
                }

                void on_receive(connection_ptr& conn_ptr, const msg_ptr& msgp) override
                {
                    uint32_t protocol_id = msgp->header().protocol_id;

                    if (protocol_id == CS_RegisterServer)
                    {
                        on_cs_register_server(conn_ptr, msgp);
                    }
                    else if (protocol_id == CS_GetServerInfo)
                    {
                        on_cs_get_server_info(conn_ptr, msgp);
                    }
                    else if (protocol_id == BroadcastServerMsg)
                    {
                        on_broadcast_server_msg(conn_ptr, msgp);
                    }
                    else if (protocol_id != SimpleRegisterServer)// !=
                    {
                        on_forward_msg(conn_ptr, msgp);
                    }
                    else
                    {
                        base_t::on_receive(conn_ptr, msgp);
                    }
                }

                connection_ptr get_connection_ptr(server_unique_id unique_id) const override
                {
                    return base_t::get_connection_ptr(unique_id);
                }

            protected:
                template<typename MSG_ID, typename T>
                void broadcast_msg(server_unique_id exclude_unique_id, MSG_ID msg_id, const T& t)
                {
                    for (auto& p : servers_)
                    {
                        if (p.first == exclude_unique_id)
                            continue;

                        auto& info_list = p.second;
                        for (auto& server : info_list)
                        {
                            send_server_msg(server.conn_ptr, msg_id, t);
                        }
                    }
                }

                void forward_msg(server_unique_id to_unique_id, msg_ptr msgp)
                {
                    auto it = servers_.find(to_unique_id);
                    if (it != servers_.end())
                    {
                        auto& info_list = it->second;
                        for (auto& server : info_list)
                        {
                            auto conn_ptr = get_running_conn(server.conn_ptr);
                            if (conn_ptr)
                            {
                                send_raw_msg(conn_ptr, msgp);
                            }
                        }
                    }
                }
            private:
                std::map<server_unique_id, std::vector<manage_server_info>> servers_;
            };
        }

        using server_manager = detail::server_manager;
    }
}