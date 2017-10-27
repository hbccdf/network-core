#pragma once
#include "../meta/meta.hpp"
namespace cytx
{
    namespace gameserver
    {
        namespace detail
        {
            enum MsgId
            {
                SimpleRegisterServer = 0x40a0009,
                BroadcastMsg = 0x60a001a,

                BroadcastServerMsg,     //向服务器广播消息
                CS_RegisterServer,      //注册服务，类型，ip，端口
                SC_RegisterServer,      //回复自动生成的id

                CS_GetServerInfo,
                SC_GetServerInfo,
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
        }
    }
}