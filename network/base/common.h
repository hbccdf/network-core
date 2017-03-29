#pragma once
#include <cstdint>
#include "utils.hpp"

namespace cytx {
    namespace rpc
    {
        // 4 byte
        struct full_procotol
        {
            uint16_t id;
            uint8_t server_type;
            bool need_reply : 1;
            bool is_reply : 1;
            uint8_t game_type : 6;
        };

        union common_protocol
        {
            uint32_t id;
            full_procotol full_proto;
        };

        inline uint32_t to_protocol(full_procotol proto)
        {
            common_protocol cp;
            cp.full_proto = proto;
            return cp.id;
        }

        inline full_procotol to_protocol(uint32_t id)
        {
            common_protocol cp;
            cp.id = id;
            return cp.full_proto;
        }

        inline uint32_t reply_protocol(uint32_t proto)
        {
            full_procotol full = to_protocol(proto);
            full.is_reply = true;
            full.need_reply = false;
            return to_protocol(full);
        }

        template<typename enum_t>
        inline auto reply_protocol(enum_t proto)->std::enable_if_t<std::is_enum<enum_t>::value, enum_t>
        {
            return (enum_t)reply_protocol((uint32_t)proto);
        }

        // 12 byte
        struct client_msg_header
        {
        private:
            static bool internal_big_endian(bool* is_big_endian = nullptr)
            {
                static bool big_endian = false;
                if (is_big_endian)
                    big_endian = *is_big_endian;
                return big_endian;
            }

        public:
            static bool big_endian()
            {
                return internal_big_endian();
            }

            static void big_endian(bool is_big_endian)
            {
                internal_big_endian(&is_big_endian);
            }

            uint16_t result_;
            uint16_t reserve_bytes_;
            uint32_t protocol_id;
            uint32_t length_;

            void ntoh()
            {
                if (!big_endian())
                    return;
                result_ = cytx::rpc::ntoh(result_);
                reserve_bytes_ = cytx::rpc::ntoh(reserve_bytes_);
                protocol_id = cytx::rpc::ntoh(protocol_id);
                length_ = cytx::rpc::ntoh(length_);
            }

            void hton()
            {
                if (!big_endian())
                    return;
                result_ = cytx::rpc::hton(result_);
                reserve_bytes_ = cytx::rpc::hton(reserve_bytes_);
                protocol_id = cytx::rpc::hton(protocol_id);
                length_ = cytx::rpc::hton(length_);
            }

            client_msg_header()
                : result_(0)
                , reserve_bytes_(0)
                , protocol_id(0)
                , length_(0)
            {
            }

            client_msg_header(uint16_t code, uint32_t protocol, uint32_t len)
                : result_(code)
                , reserve_bytes_(0)
                , protocol_id(protocol)
                , length_(len)
            {
            }

            bool is_internal() const
            {
                return false;
            }

            bool is_reply() const
            {
                auto proto = to_protocol(protocol_id);
                return proto.is_reply;
            }

            void reply(bool is_reply)
            {
                protocol_id = reply_protocol(protocol_id);
            }

            bool need_reply() const
            {
                auto proto = to_protocol(protocol_id);
                return (!proto.is_reply) && proto.need_reply;
            }

            uint16_t result() const
            {
                return result_;
            }
            void result(uint16_t r)
            {
                result_ = r;
            }

            uint32_t call_id() const
            {
                return protocol_id;
            }

            uint32_t length() const
            {
                return this->length_;
            }
            void length(uint32_t len)
            {
                this->length_ = len;
            }

            uint32_t proto() const
            {
                return protocol_id;
            }
        };

        // 24 byte
        struct server_msg_header
        {
        private:
            static bool internal_big_endian(bool* is_big_endian = nullptr)
            {
                static bool big_endian = false;
                if (is_big_endian)
                    big_endian = *is_big_endian;
                return big_endian;
            }

        public:
            static bool big_endian()
            {
                return internal_big_endian();
            }

            static void big_endian(bool is_big_endian)
            {
                internal_big_endian(&is_big_endian);
            }

            server_msg_header() = default;

            server_msg_header(uint16_t from_id, uint16_t to_id, uint32_t conn_id,
                uint32_t call_id, uint16_t code, uint32_t protocol, uint32_t len)
                : from_unique_id(from_id)
                , to_unique_id(to_id)
                , conn_id(conn_id)
                , call_id(call_id)

                , result(code)
                , protocol_id(protocol)
                , length(len)
                , reserve_bytes(0)
            {}

            uint16_t from_unique_id;
            uint16_t to_unique_id;
            uint32_t conn_id;
            uint32_t call_id;

            uint16_t result;
            uint16_t reserve_bytes;
            uint32_t protocol_id;
            uint32_t length;

            void ntoh()
            {
                if (!big_endian())
                    return;
                from_unique_id = cytx::rpc::ntoh(from_unique_id);
                to_unique_id = cytx::rpc::ntoh(to_unique_id);
                conn_id = cytx::rpc::ntoh(conn_id);
                call_id = cytx::rpc::ntoh(call_id);

                result = cytx::rpc::ntoh(result);
                reserve_bytes = cytx::rpc::ntoh(reserve_bytes);
                protocol_id = cytx::rpc::ntoh(protocol_id);
                length = cytx::rpc::ntoh(length);
            }

            void hton()
            {
                if (!big_endian())
                    return;
                from_unique_id = cytx::rpc::hton(from_unique_id);
                to_unique_id = cytx::rpc::hton(to_unique_id);
                conn_id = cytx::rpc::hton(conn_id);
                call_id = cytx::rpc::hton(call_id);

                result = cytx::rpc::hton(result);
                reserve_bytes = cytx::rpc::hton(reserve_bytes);
                protocol_id = cytx::rpc::hton(protocol_id);
                length = cytx::rpc::hton(length);
            }

            void init(uint16_t from_id, uint16_t to_id, uint32_t conn_id,
                uint32_t call_id, uint16_t code, uint32_t protocol, uint32_t len)
            {
                this->from_unique_id = from_id;
                this->to_unique_id = to_id;
                this->conn_id = conn_id;
                this->call_id = call_id;
                this->result = code;
                this->reserve_bytes = 0;
                this->length = len;
                this->protocol_id = protocol;
            }

            void init(uint16_t code, uint32_t protocol, uint32_t len)
            {
                this->from_unique_id = 0;
                this->to_unique_id = 0;
                this->conn_id = 0;
                this->call_id = 0;
                this->result = code;
                this->reserve_bytes = 0;
                this->length = len;
                this->protocol_id = protocol;
            }

        };

        enum class msg_way
        {
            one_way,
            two_way,
        };

        // 2 byte
        struct msg_option
        {
            bool is_reply : 1;
            bool is_one_way : 1;
            int8_t protocol_type : 2;
            int8_t reserve : 4;
            int8_t reserve1;
        };

        // 24 byte
        struct internal_msg_header
        {
            internal_msg_header() = default;

            internal_msg_header(int16_t code, msg_option option, int32_t call_id, uint32_t len, uint64_t hash)
                : flag(65535)
                , result(code)
                , option(option)
                , call_id(call_id)
                , len(len)
                , hash(hash)
                , reserve_bytes(0)
            {}

            uint16_t flag;
            uint16_t result;
            msg_option option;
            uint16_t reserve_bytes;
            uint32_t call_id;
            uint32_t len;
            uint64_t hash;

            void ntoh()
            {
            }

            void hton()
            {
            }

            void init(int16_t code, msg_option option, int32_t call_id, uint32_t len, uint64_t hash)
            {
                flag = 65535;
                result = code;
                this->option = option;
                this->call_id = call_id;
                this->len = len;
                this->hash = hash;
                this->reserve_bytes = 0;
            }
        };

        union common_msg_header
        {
            uint16_t flag;
            server_msg_header smh;
            internal_msg_header imh;
        };

        struct msg_header
        {
            static bool big_endian() { return server_msg_header::big_endian(); }

            msg_header() = default;

            msg_header(uint16_t code, msg_option option, uint32_t call_id, uint32_t len, uint64_t hash)
            {
                h.imh.init(code, option, call_id, len, hash);
            }

            msg_header(uint16_t from_id, uint16_t to_id, uint32_t conn_id,
                uint32_t call_id, uint16_t code, uint32_t protocol, uint32_t len)
            {
                h.smh.init(from_id, to_id, conn_id, call_id, code, protocol, len);
            }

            msg_header(uint16_t code, uint32_t protocol, uint32_t len)
            {
                h.smh.init(code, protocol, len);
            }

            common_msg_header h;

            server_msg_header& server_header()
            {
                return h.smh;
            }

            internal_msg_header& internal_header()
            {
                return h.imh;
            }

            const server_msg_header& server_header() const
            {
                return h.smh;
            }

            const internal_msg_header& internal_header() const
            {
                return h.imh;
            }

            bool is_internal() const
            {
                return h.flag == 65535;
            }

            bool is_reply() const
            {
                if (is_internal())
                    return internal_header().option.is_reply;
                else
                {
                    auto proto = to_protocol(server_header().protocol_id);
                    return proto.is_reply;
                }
            }

            void reply(bool is_reply)
            {
                if (is_internal())
                    internal_header().option.is_reply = is_reply;
                else
                {
                    server_header().protocol_id = reply_protocol(server_header().protocol_id);
                }
            }

            bool need_reply() const
            {
                if (is_internal())
                    return !internal_header().option.is_one_way;
                else
                {
                    auto proto = to_protocol(server_header().protocol_id);
                    return (!proto.is_reply) && proto.need_reply;
                }
            }

            uint16_t result() const
            {
                return is_internal() ? internal_header().result : server_header().result;
            }
            void result(uint16_t r)
            {
                if (is_internal())
                    internal_header().result = r;
                else
                    server_header().result = r;
            }

            uint32_t call_id() const
            {
                return is_internal() ? internal_header().call_id : server_header().call_id;
            }
            void call_id(uint32_t id)
            {
                if (is_internal())
                    internal_header().call_id = id;
                else
                    server_header().call_id = id;
            }

            uint32_t length() const
            {
                return is_internal() ? internal_header().len : server_header().length;
            }
            void length(uint32_t len)
            {
                if (is_internal())
                    internal_header().len = len;
                else
                    server_header().length = len;
            }

            uint64_t proto() const
            {
                return is_internal() ? internal_header().hash : server_header().protocol_id;
            }

            void ntoh()
            {
                if (is_internal())
                    internal_header().ntoh();
                else
                    server_header().ntoh();
            }

            void hton()
            {
                if (is_internal())
                    internal_header().hton();
                else
                    server_header().hton();
            }
        };

        struct blob_t
        {
            blob_t() : ptr_(nullptr), size_(0) {}
            blob_t(char const* data, size_t size)
                : ptr_(data), size_(size)
            {
            }
            auto data() const
            {
                return ptr_;
            }

            size_t size() const
            {
                return size_;
            }

            char const* ptr_ = nullptr;
            size_t size_ = 0;
        };

        enum class result_code : int16_t
        {
            ok = 0,
            fail = 100,
        };

        enum class error_code
        {
            ok,
            unknown,
            fail,
            timeout,
            cancel,
            connect_fail,
            badconnect,
            codec_fail,
            invalid_header,
            be_disconnected,
        };
    }
}

