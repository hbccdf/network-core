#pragma once
#include "../base/common.h"
#include "../base/utils.hpp"
#include "../base/GameObjectStream.h"
#include <boost/asio/buffer.hpp>

namespace cytx {
    namespace gameserver {

        using stream_t = cytx::GameObjectStream;

        struct msg_header
        {
            using this_t = msg_header;
        private:
            static bool internal_big_endian(bool* is_big_endian = nullptr)
            {
                static bool _big_endian = false;
                if (is_big_endian)
                    _big_endian = *is_big_endian;
                return _big_endian;
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

        public:

            uint16_t from_unique_id;
            uint16_t to_unique_id;
            union
            {
                uint32_t conn_id;
                uint32_t user_id;
            };
            uint32_t call_id;

            uint16_t result;
            uint16_t reserve_bytes;
            uint32_t protocol_id;
            uint32_t length;

            msg_header() = default;

            msg_header(uint16_t from_id, uint16_t to_id, uint32_t conn_id,
                uint32_t call_id, uint16_t code, uint32_t protocol, uint32_t len)
            {
                init(from_id, to_id, conn_id, call_id, code, protocol, len);
            }

            msg_header(uint16_t code, uint32_t protocol, uint32_t len)
            {
                init(code, protocol, len);
            }

            void ntoh()
            {
                if (!big_endian())
                    return;

                from_unique_id = cytx::util::ntoh(from_unique_id);
                to_unique_id = cytx::util::ntoh(to_unique_id);
                conn_id = cytx::util::ntoh(conn_id);
                call_id = cytx::util::ntoh(call_id);

                result = cytx::util::ntoh(result);
                reserve_bytes = cytx::util::ntoh(reserve_bytes);
                protocol_id = cytx::util::ntoh(protocol_id);
                length = cytx::util::ntoh(length);
            }

            void hton()
            {
                if (!big_endian())
                    return;

                from_unique_id = cytx::util::hton(from_unique_id);
                to_unique_id = cytx::util::hton(to_unique_id);
                conn_id = cytx::util::hton(conn_id);
                call_id = cytx::util::hton(call_id);

                result = cytx::util::hton(result);
                reserve_bytes = cytx::util::hton(reserve_bytes);
                protocol_id = cytx::util::hton(protocol_id);
                length = cytx::util::hton(length);
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

            boost::asio::const_buffer to_buffer() const
            {
                return boost::asio::buffer(this, sizeof(this_t));
            }
        };

        struct msg_body
        {
            char* data_;
            uint32_t length_;
            uint32_t offset_;

            msg_body() : data_(nullptr), length_(0), offset_(0) {}

            msg_body(const msg_body& other)
            {
            }

            ~msg_body()
            {
                if (data_)
                {
                    DELETE_ARRAY_MP(char, data_, length_);
                }
            }

            void alloc(int data_size)
            {
                length_ = data_size;
                offset_ = 0;
                data_ = NEW_ARRAY_MP(char, length_);
            }

            void reset(char* data_ptr, int data_size)
            {
                if (data_)
                {
                    DELETE_ARRAY_MP(char, data_, length_);
                }
                data_ = data_ptr;
                length_ = data_size;
                offset_ = 0;
            }

            char* raw_data() const
            {
                return data_;
            }

            char* data() const
            {
                return data_ + offset_;
            }

            uint32_t raw_length() const { return length_; }
            uint32_t length() const { return length_ - offset_; }

            boost::asio::const_buffer to_buffer() const
            {
                return boost::asio::buffer(data(), length());
            }
        };

        template<typename T>
        struct basic_server_msg;

        template<>
        struct basic_server_msg<msg_body>
        {
            using header_t = msg_header;
            using body_t = msg_body;

            header_t header_;
            body_t body_;

            basic_server_msg()
                : header_(), body_()
            {}

            basic_server_msg(const header_t& h)
                : header_(h), body_()
            {
                if (h.length > 0)
                {
                    body_.alloc((int32_t)h.length);
                }
            }

            void reset(char* data_ptr, int data_size)
            {
                header_.length = (uint32_t)data_size;
                body_.reset(data_ptr, data_size);
            }

            void reset(stream_t& gos)
            {
                header_.length = (uint32_t)gos.length();
                body_.reset(gos.data(), gos.length());
                gos.set_alloc_type(0);
            }

            char* raw_data() const
            {
                return body_.raw_data();
            }

            char* data() const
            {
                return body_.data();
            }

            uint32_t raw_length() const { return body_.raw_length(); }
            uint32_t length() const { return body_.length(); }

            const header_t& header() const { return header_; }
            header_t& header() { return header_; }

            void hton()
            {
                header_.hton();
            }

            void ntoh()
            {
                header_.ntoh();
            }

            std::vector<boost::asio::const_buffer> to_buffers() const
            {
                std::vector<boost::asio::const_buffer> buffers;
                to_buffers(buffers);
                return buffers;
            }

            void to_buffers(std::vector<boost::asio::const_buffer>& buffers) const
            {
                buffers.emplace_back(header_.to_buffer());
                buffers.emplace_back(body_.to_buffer());
            }

            uint32_t total_length() const
            {
                return (uint32_t)sizeof(header_t) + header_.length;
            }

            stream_t get_stream() const
            {
                stream_t gos(data(), (int)length(), 0);
                return gos;
            }
        };

        template<typename T>
        using server_msg = basic_server_msg<T>;

        template<typename T>
        class batch_msg
        {
            using msg_t = T;
            using msg_ptr = std::shared_ptr<msg_t>;
        public:
            batch_msg(size_t capacity, size_t length_capacity)
                : capacity_(capacity)
                , length_capacity_(length_capacity)
                , cur_length_(0)
            {

            }
            batch_msg()
                : batch_msg(0, 0)
            {

            }

            bool full() const
            {
                return capacity_ > 0 && msgs_.size() >= capacity_;
            }

            bool empty() const
            {
                return msgs_.empty();
            }

            size_t size() const
            {
                return msgs_.size();
            }

            bool length_full() const
            {
                return length_capacity_ > 0 && cur_length_ >= length_capacity_;
            }

            size_t total_length() const
            {
                return cur_length_;
            }

            bool add_msg(msg_ptr msg)
            {
                if (full() || length_full())
                    return false;

                msgs_.push_back(msg);
                cur_length_ += msg->total_length();

                return true;
            }

            void hton()
            {
                for (auto& msg : msgs_)
                {
                    msg->hton();
                }
            }

            void ntoh()
            {
                for (auto& msg : msgs_)
                {
                    msg->ntoh();
                }
            }

            std::vector<boost::asio::const_buffer> to_buffers() const
            {
                std::vector<boost::asio::const_buffer> buffers;
                for (auto& msg : msgs_)
                {
                    msg->to_buffers(buffers);
                }
                return buffers;
            }
        private:
            std::vector<msg_ptr> msgs_;
            size_t capacity_;
            size_t length_capacity_;
            size_t cur_length_;
        };
    }
}