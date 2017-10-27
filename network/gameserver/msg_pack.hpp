#pragma once

#include "raw_msg.hpp"
#include "../codec/codec_gos.hpp"
#include "../codec/codec_thrift.hpp"

namespace cytx
{
    namespace gameserver
    {
        namespace detail
        {
            using gos_t = cytx::GameObjectStream;
            using thrift_base_t = ::apache::thrift::TBase;
            using buffer_t = cytx::codec::gos_buffer;

            using msg_t = cytx::gameserver::server_msg<cytx::gameserver::msg_body>;
            using msg_ptr = std::shared_ptr<msg_t>;
            using header_t = typename msg_t::header_t;

            template <typename T, class = std::void_t<>>
            struct is_thrift_msg : std::false_type
            {
            };

            template<typename T>
            struct is_thrift_msg<T, std::void_t<std::enable_if_t<std::is_base_of<thrift_base_t, T>::value>>> : std::true_type
            {

            };

            template <typename T, class = std::void_t<>>
            struct is_gos_msg : std::false_type
            {
            };

            template<typename T>
            struct is_gos_msg<T, std::void_t<std::enable_if_t<!is_thrift_msg<T>::value>>> : std::true_type
            {

            };

            /*template <typename T>
            struct has_pack_method
            {
            private:
                template <typename P, typename = decltype(std::declval<P>().Meta())>
                static std::true_type test(int);
                template <typename P>
                static std::false_type test(...);
                using result_type = decltype(test<T>(0));
            public:
                static constexpr bool value = result_type::value;
            };

            template <typename T, class = std::void_t<>>
            struct is_custom_msg : std::false_type
            {
            };*/

            template<typename T>
            auto pack_msg_impl(gos_t& gos, const T& t, bool is_big_endian) -> std::enable_if_t<is_thrift_msg<T>::value>
            {
                cytx::codec::thrift_codec codec{ is_big_endian };
                codec.pack(gos, t);
            }

            template<typename T>
            auto pack_msg_impl(gos_t& gos, const T& t, bool is_big_endian) -> std::enable_if_t<is_gos_msg<T>::value>
            {
                cytx::codec::gos_codec codec{ is_big_endian };
                codec.pack(gos, t);
            }

            template<typename T>
            auto pack_msg_impl(const T& t, bool is_big_endian) -> std::enable_if_t<is_thrift_msg<T>::value, buffer_t>
            {
                cytx::codec::thrift_codec codec{ is_big_endian };
                return codec.pack(t);
            }

            template<typename T>
            auto pack_msg_impl(const T& t, bool is_big_endian) -> std::enable_if_t<is_gos_msg<T>::value, buffer_t>
            {
                cytx::codec::gos_codec codec{ is_big_endian };
                return codec.pack(t);
            }

            template<typename T>
            auto unpack_msg_impl(char const* data, size_t length, bool is_big_endian) ->std::enable_if_t<is_thrift_msg<T>::value, T>
            {
                cytx::codec::thrift_codec codec{ is_big_endian };
                return codec.unpack<T>(data, length);
            }

            template<typename T>
            auto unpack_msg_impl(char const* data, size_t length, bool is_big_endian) ->std::enable_if_t<is_gos_msg<T>::value, T>
            {
                cytx::codec::gos_codec codec{ is_big_endian };
                return codec.unpack<T>(data, length);
            }
        }

        template<typename T>
        detail::msg_ptr pack_msg(const T& t, bool is_big_endian = true)
        {
            using namespace detail;
            msg_ptr msg = std::make_shared<msg_t>();
            buffer_t buffer = detail::pack_msg_impl<T>(t, is_big_endian);
            msg->reset(buffer.data(), (int)buffer.size());
            buffer.reset();
            return msg;
        }

        template<typename T>
        void pack_msg(detail::gos_t& gos, const T& t, bool is_big_endian = true)
        {
            detail::pack_msg_impl<T>(gos, t, is_big_endian);
        }

        template<typename ... ARGS>
        detail::msg_ptr pack_msg(bool is_big_endian, const ARGS& ... args)
        {
            using namespace detail;
            msg_ptr msg = std::make_shared<msg_t>();
            gos_t gos;

            char a[] = { (detail::pack_msg_impl(gos, args, is_big_endian), 0) ... };

            msg->reset(gos.data_, gos.length());
            gos.alloc_type_ = 0;
            return msg;
        }

        template<typename T>
        T unpack_msg(detail::msg_ptr msgp)
        {
            return detail::unpack_msg_impl<T>(msgp->data(), msgp->length(), detail::header_t::big_endian());
        }
    }
}