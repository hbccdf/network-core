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
                    info.connect_interval = 4999;
                    servers_[data.unique_id].emplace_back(server_info);

                    conn_ptr->set_conn_info(connection_info{ data.unique_id });
                    LOG_DEBUG("server {} register", (uint16_t)data.unique_id);

                    //通知其他服务
                    SCServerEvent server_event;
                    server_event.event = EServerConnected;
                    server_event.info = server_info.info;
                    for (auto& p : servers_)
                    {
                        /*if (p.first == data.unique_id)
                            continue;*/

                        auto& info_list = p.second;
                        for (auto& server : info_list)
                        {
                            send_server_msg(server.conn_ptr, SC_ServerEvent, server_event, msgp);
                        }
                    }
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
                    for (auto unique_id : data.servers)
                    {
                        for (auto& p : servers_)
                        {
                            if (p.first != unique_id)
                                continue;

                            auto& info_list = p.second;
                            for (auto& server : info_list)
                            {
                                send_raw_msg(server.conn_ptr, send_msgp);
                            }
                        }
                    }
                }

            protected:
                void on_disconnect(connection_ptr& conn_ptr, const net_result& err) override
                {
                    server_unique_id unique_id = conn_ptr->get_conn_info().unique_id;

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
                            LOG_ERROR("server {} disconnect", (uint16_t)unique_id);
                            server_info_list.erase(info_it);
                            break;
                        }
                    }

                    //通知其他服务
                    SCServerEvent server_event;
                    server_event.event = EServerDisconnected;
                    server_event.info.unique_id = unique_id;
                    for (auto& p : servers_)
                    {
                        if(p.first == unique_id)
                            continue;

                        auto& info_list = p.second;
                        for (auto& server : info_list)
                        {
                            send_server_msg(server.conn_ptr, SC_ServerEvent, server_event);
                        }
                    }
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
                    else
                    {
                        base_t::on_receive(conn_ptr, msgp);
                    }
                }

                connection_ptr get_connection_ptr(server_unique_id unique_id) const override
                {
                    return base_t::get_connection_ptr(unique_id);
                }

            private:
                std::map<server_unique_id, std::vector<manage_server_info>> servers_;
            };
        }

        using server_manager = detail::server_manager;
    }
}