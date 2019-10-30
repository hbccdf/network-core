﻿#pragma once
#include "network/meta/meta.hpp"
namespace cytx
{
    namespace gameserver
    {
        namespace detail
        {
            enum MsgId
            {
                //Gateway msg ids
                SimpleRegisterServer = 0x40a0009,
                BroadcastMsg = 0x60a001a,

                //center msg ids, ids is simple msg id
                BroadcastServerMsg = 170000,    //向服务器广播消息
                CS_RegisterServer = 170001,     //注册服务，类型，ip，端口
                SC_RegisterServer = 170002,     //回复自动生成的id

                CS_GetServerInfo = 170003,      //获取服务信息
                SC_GetServerInfo = 170004,

                SC_ServerEvent = 170006,
            };

            using InterMsgId = MsgId;
            REG_ALIAS_ENUM(InterMsgId, proto, BroadcastServerMsg, CS_RegisterServer, SC_RegisterServer, CS_GetServerInfo, SC_GetServerInfo, SC_ServerEvent);

            enum EServerEvent
            {
                EServerConnected,
                EServerDisconnected,
            };

            struct BroadcastInfo
            {
                uint32_t msg_id;
                std::vector<server_unique_id> servers;
                META(msg_id, servers);
            };

            struct ServerInfo
            {
                server_unique_id unique_id;
                std::string ip;
                uint16_t port;
                int32_t connect_interval;
                META(unique_id, ip, port, connect_interval);
            };

            struct CSRegisterServer
            {
                server_unique_id unique_id;
                std::string ip;
                uint16_t port;
                META(unique_id, ip, port);
            };

            struct SCRegisterServer
            {
                uint16_t server_uid;
                META(server_uid);
            };

            struct CSGetServerInfo
            {
                std::vector<server_unique_id> servers;
                META(servers);
            };

            struct SCGetServerInfo
            {
                std::map<server_unique_id, ServerInfo> servers;
                META(servers);
            };

            struct SCServerEvent
            {
                EServerEvent event;
                ServerInfo info;
                META(event, info);
            };
        }
    }
}