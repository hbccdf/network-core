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

            using msg_t = server_msg<msg_body>;
            using header_t = typename msg_t::header_t;
            using connection_t = tcp_connection<msg_t>;
            using connection_ptr = std::shared_ptr<connection_t>;
            using msg_ptr = typename connection_t::msg_ptr;
            using proto_t = Proto;
            using proto_ptr_t = std::shared_ptr<proto_t>;
            using proto_map_t = std::map<uint32_t, proto_ptr_t>;

            class Proto : public std::enable_shared_from_this<Proto>
            {
                friend void set_proto_id(Proto* proto, uint32_t proto_id);
            public:
                Proto()
                    : protocol_id_(0)
                {}
                Proto(const Proto& rhs)
                    : protocol_id_(rhs.protocol_id_)
                {
                }
                uint32_t get_protocol_id() const { return protocol_id_; }
            public:
                virtual proto_ptr_t clone() = 0;
                virtual msg_ptr pack() = 0;
                virtual void unpack(msg_ptr& msgp) = 0;
                virtual void process(msg_ptr& msgp, connection_ptr& conn_ptr) = 0;
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
                static proto_ptr_t Decode(msg_ptr msgp)
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
            private:
                Proto(uint32_t id)
                    : protocol_id_(id)
                {}

                void set_protocol_id(uint32_t proto_id)
                {
                    protocol_id_ = proto_id;
                }
                uint32_t protocol_id_;
            };

            template <typename T, class = std::void_t<>>
            struct is_Proto_msg : std::false_type
            {
            };

            template<typename T>
            struct is_Proto_msg<T, std::void_t<std::enable_if_t<std::is_base_of<proto_t, T>::value>>> : std::true_type
            {

            };

            inline void set_proto_id(Proto* proto, uint32_t proto_id)
            {
                proto->set_protocol_id(proto_id);
            }

            template<typename T>
            auto make_proto_ptr(uint32_t proto_id) -> std::enable_if_t<is_Proto_msg<T>::value, std::shared_ptr<T>>
            {
                std::shared_ptr<T> ptr = std::make_shared<T>();
                set_proto_id(ptr.get(), proto_id);
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
        using connection_ptr = std::shared_ptr<connection_t>;
        using msg_ptr = typename connection_t::msg_ptr;

        template<typename T>
        auto pack_msg(bool is_big_endian, const T& t) -> std::enable_if_t<detail::is_Proto_msg<T>::value, detail::msg_ptr>
        {
            return t.pack();
        }
    }
}


#define REGISTER_PROTOCOL(type) \
namespace ___reg_proto_helper_value___ ## type ## __LINE__  \
{ \
  static auto type ## _ptr = cytx::gameserver::detail::make_proto_ptr<type>(type::ProtoId());   \
}