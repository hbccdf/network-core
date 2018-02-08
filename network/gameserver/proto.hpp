#pragma once
#include "tcp_connect.hpp"
#include "msg_pack.hpp"

namespace cytx
{
    namespace gameserver
    {
        namespace detail
        {
            using namespace cytx;
            using namespace cytx::gameserver;

            class Proto;
            class game_server;
            using game_server_t = game_server;

            using msg_t = server_msg<msg_body>;
            using header_t = typename msg_t::header_t;
            using connection_t = tcp_connection<msg_t>;
            using connection_ptr = std::shared_ptr<connection_t>;
            using context_t = typename connection_t::context_t;
            using msg_ptr = typename connection_t::msg_ptr;
            using proto_t = Proto;
            using proto_ptr_t = std::shared_ptr<proto_t>;
            using proto_map_t = std::map<uint32_t, proto_ptr_t>;

            template <typename T>
            struct has_ctx_process_func
            {
            private:
                template <typename P, typename = decltype(std::declval<P>().process(*(msg_ptr)nullptr, *(connection_ptr*)nullptr, *(game_server_t*)nullptr, *(context_t*)nullptr))>
                static std::true_type test(int);
                template <typename P>
                static std::false_type test(...);
                using result_type = decltype(test<T>(0));
            public:
                static constexpr bool value = result_type::value;
            };
            template<typename T>
            constexpr bool has_ctx_process_func_v = has_ctx_process_func<T>::value;

            class Proto : public std::enable_shared_from_this<Proto>, public custom_msg
            {
            public:
                Proto()
                    : protocol_id_(0)
                    , has_ctx_process_(false)
                {}
                Proto(const Proto& rhs)
                    : protocol_id_(rhs.protocol_id_)
                    , has_ctx_process_(rhs.has_ctx_process_)
                {
                }
                uint32_t get_protocol_id() const { return protocol_id_; }
                void set_has_ctx_process(bool has_ctx_process) { has_ctx_process_ = has_ctx_process; }
            public:
                virtual proto_ptr_t clone()
                {
                    return nullptr;
                }
                virtual void unpack(msg_ptr& msgp) = 0;

                void do_process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
                {
                    if (!has_ctx_process_)
                    {
                        process(msgp, conn_ptr, server);
                    }
                    else
                    {
                        ios_t& ios = get_current_ios();
                        boost::asio::spawn(ios, [&] (context_t ctx)
                        {
                            this->process(msgp, conn_ptr, server, ctx);
                        });
                    }
                }
            private:
                virtual void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server)
                {
                }

                virtual void process(msg_ptr& msgp, connection_ptr& conn_ptr, game_server_t& server, context_t ctx)
                {
                }
            public:
                static proto_map_t& GetMap()
                {
                    static proto_map_t protocols;
                    return protocols;
                }
                static proto_ptr_t GetProto(uint32_t protocol_id)
                {
                    auto& proto_map = GetMap();
                    auto it = proto_map.find(protocol_id);
                    if (it != proto_map.end())
                    {
                        return it->second;
                    }
                    return nullptr;
                }
                static proto_ptr_t Decode(msg_ptr& msgp)
                {
                    uint32_t protocol_id = msgp->header().protocol_id;
                    proto_ptr_t proto = Create(protocol_id);
                    if (proto)
                    {
                        proto->unpack(msgp);
                    }
                    return proto;
                }
                static proto_ptr_t Create(uint32_t protocol_id)
                {
                    proto_ptr_t proto = GetProto(protocol_id);
                    return proto ? proto->clone() : nullptr;
                }

                static void RegisterProto(proto_ptr_t proto)
                {
                    uint32_t proto_id = proto->get_protocol_id();
                    if (!GetProto(proto_id))
                    {
                        auto& proto_map = GetMap();
                        proto_map.emplace(proto_id, proto);
                    }
                }
            protected:
                Proto(uint32_t id)
                    : protocol_id_(id)
                    , has_ctx_process_(false)
                {}

                uint32_t protocol_id_;
                bool has_ctx_process_;
            };

            template<typename T>
            auto make_proto_ptr(uint32_t proto_id)
            {
                bool has_ctx_process = has_ctx_process_func_v<T>;
                std::shared_ptr<T> ptr = std::make_shared<T>();
                ptr->set_has_ctx_process(has_ctx_process);
                proto_t::RegisterProto(ptr);
                return ptr;
            }
        }

        using Proto = detail::Proto;
        using proto_t = Proto;
        using proto_ptr_t = std::shared_ptr<proto_t>;
        using msg_t = server_msg<msg_body>;
        using header_t = typename msg_t::header_t;
        using connection_t = tcp_connection<msg_t>;
        using context_t = typename connection_t::context_t;
        using connection_ptr = std::shared_ptr<connection_t>;
        using msg_ptr = typename connection_t::msg_ptr;
    }
}


#define REGISTER_PROTOCOL(type) \
namespace ___reg_proto_helper_value___ ## type ## __LINE__  \
{ \
  static auto type ## _ptr = cytx::gameserver::detail::make_proto_ptr<type>(type::ProtoId());   \
}

