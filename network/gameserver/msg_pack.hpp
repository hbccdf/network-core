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

            class custom_msg{};

            template <typename T, class = std::void_t<>>
            struct is_custom_msg : std::false_type
            {
            };

            template<typename T>
            struct is_custom_msg<T, std::void_t<std::enable_if_t<std::is_base_of<custom_msg, T>::value>>> : std::true_type
            {

            };

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
            struct is_gos_msg<T, std::void_t<std::enable_if_t<!is_thrift_msg<T>::value && !is_custom_msg<T>::value>>> : std::true_type
            {

            };

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
            auto pack_msg_impl(gos_t& gos, const T& t, bool is_big_endian) -> std::enable_if_t<is_custom_msg<T>::value>
            {
                t.pack(gos);
            }

            inline void pack_msg_impl(gos_t& gos, const msg_ptr& msgp, bool is_big_endian)
            {
                gos.pushBinary(msgp->data(), msgp->length());
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
            auto pack_msg_impl(const T& t, bool is_big_endian) -> std::enable_if_t<is_custom_msg<T>::value, buffer_t>
            {
                gos_t gos;
                t.pack(gos);
                buffer_t buffer(gos);
                return buffer;
            }

            template<typename T>
            auto unpack_msg_impl(const char* data, size_t length, bool is_big_endian) ->std::enable_if_t<is_thrift_msg<T>::value, T>
            {
                cytx::codec::thrift_codec codec{ is_big_endian };
                return codec.unpack<T>(data, length);
            }

            template<typename T>
            auto unpack_msg_impl(const char* data, size_t length, bool is_big_endian) ->std::enable_if_t<is_gos_msg<T>::value, T>
            {
                cytx::codec::gos_codec codec{ is_big_endian };
                return codec.unpack<T>(data, length);
            }

            template<typename T>
            auto unpack_msg_impl(gos_t& gos, bool is_big_endian) ->std::enable_if_t<is_thrift_msg<T>::value, T>
            {
                cytx::codec::thrift_codec codec{ is_big_endian };
                return codec.unpack<T>(gos);
            }

            template<typename T>
            auto unpack_msg_impl(gos_t& gos, bool is_big_endian) ->std::enable_if_t<is_gos_msg<T>::value, T>
            {
                cytx::codec::gos_codec codec{ is_big_endian };
                return codec.unpack<T>(gos);
            }

            template<typename T>
            auto unpack_msg_impl(gos_t& gos, T& t, bool is_big_endian) ->std::enable_if_t<is_thrift_msg<T>::value>
            {
                cytx::codec::thrift_codec codec{ is_big_endian };
                t = codec.unpack<T>(gos);
            }

            template<typename T>
            auto unpack_msg_impl(gos_t& gos, T& t, bool is_big_endian) ->std::enable_if_t<is_gos_msg<T>::value>
            {
                cytx::codec::gos_codec codec{ is_big_endian };
                t = codec.unpack<T>(gos);
            }
        }

        template<typename T>
        auto pack_msg(const T& t) -> std::enable_if_t<!detail::is_custom_msg<T>::value, detail::msg_ptr>
        {
            detail::msg_ptr msg = std::make_shared<detail::msg_t>();
            detail::buffer_t buffer = detail::pack_msg_impl<T>(t, detail::header_t::big_endian());
            msg->reset(buffer.data(), (int)buffer.size());
            buffer.reset();
            return msg;
        }

        template<typename T>
        auto pack_msg(const T& t) -> std::enable_if_t<detail::is_custom_msg<T>::value, detail::msg_ptr>
        {
            return t.pack();
        }

        template<typename T>
        void pack_msg(detail::gos_t& gos, const T& t)
        {
            detail::pack_msg_impl<T>(gos, t, detail::header_t::big_endian());
        }

        template<typename ... ARGS>
        detail::msg_ptr pack_msg(const ARGS& ... args)
        {
            bool is_big_endian = detail::header_t::big_endian();
            detail::msg_ptr msg = std::make_shared<detail::msg_t>();
            detail::gos_t gos;

            int a[] = { (detail::pack_msg_impl(gos, args, is_big_endian), 0) ... };

            msg->reset(gos);
            return msg;
        }

        template<typename ... ARGS>
        void pack_msg(detail::gos_t& gos, const ARGS& ... args)
        {
            bool is_big_endian = detail::header_t::big_endian();

            int a[] = { (detail::pack_msg_impl(gos, args, is_big_endian), 0) ... };
        }

        inline detail::msg_ptr pack_msg()
        {
            detail::msg_ptr msg = std::make_shared<detail::msg_t>();
            return msg;
        }

        template<typename T>
        auto unpack_msg(detail::msg_ptr msgp) -> std::enable_if_t<!detail::is_custom_msg<T>::value, T>
        {
            return detail::unpack_msg_impl<T>(msgp->data(), msgp->length(), detail::header_t::big_endian());
        }

        template<typename T>
        auto unpack_msg(detail::msg_ptr msgp) -> std::enable_if_t<detail::is_custom_msg<T>::value, T>
        {
            T t{};
            t.unpack(msgp);
            return t;
        }

        template<typename ... ARGS>
        void unpack_msg(detail::msg_ptr msgp, ARGS& ... args)
        {
            bool is_big_endian = detail::header_t::big_endian();
            detail::gos_t gos = msgp->get_stream();

            int a[] = { (detail::unpack_msg_impl(gos, args, is_big_endian), 0) ... };
        }

        template<typename T>
        auto unpack_msg(detail::gos_t& gos)
        {
            return detail::unpack_msg_impl<T>(gos, detail::header_t::big_endian());
        }

        template<typename ... ARGS>
        void unpack_msg(detail::gos_t& gos, ARGS& ... args)
        {
            bool is_big_endian = detail::header_t::big_endian();

            int a[] = { (detail::unpack_msg_impl(gos, args, is_big_endian), 0) ... };
        }

        template<typename T>
        auto unpack_msg(const char* data, size_t length)
        {
            return detail::unpack_msg_impl<T>(data, length, detail::header_t::big_endian());
        }
    }
}