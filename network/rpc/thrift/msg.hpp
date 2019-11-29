#pragma once
#include "network/net/raw_msg.hpp"

namespace cytx
{
    namespace thrift
    {
        struct thrift_header
        {
            using this_t = thrift_header;
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

            uint32_t length;

            thrift_header() = default;

            thrift_header(uint32_t len)
            {
                init(len);
            }

            void ntoh()
            {
                if (!big_endian())
                    return;

                length = cytx::util::ntoh(length);
            }

            void hton()
            {
                if (!big_endian())
                    return;

                length = cytx::util::hton(length);
            }

            void init(uint32_t len)
            {
                this->length = len;
            }

            boost::asio::const_buffer to_buffer() const
            {
                return boost::asio::buffer(this, sizeof(this_t));
            }
        };

        using thrift_msg = net::basic_msg<thrift_header, net::msg_body>;
    }
}