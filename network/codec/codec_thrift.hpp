#pragma once
#include "network/base/excetion.hpp"
#include "network/serialize/thrift_adapter.hpp"
#include "codec_common.hpp"

namespace cytx {
    namespace codec {

        struct thrift_codec
        {
            thrift_codec(bool) {}

            template<typename T>
            T unpack(const char* data, size_t length)
            {
                try
                {
                    GameObjectStream gos(const_cast<char*>(data), (int)length, 0);
                    DeSerializer<thrift_deserialize_adapter> dr(gos);

                    T t{};
                    dr.DeSerialize(t);
                    return t;
                }
                catch (std::exception& e)
                {
                    throw cytx::net_exception(cytx::error_code::codec_fail, e.what());
                }
            }

            template<typename T, typename Tuple>
            T unpack(const char* data, size_t length, Tuple&& tuple)
            {
                try
                {
                    GameObjectStream gos(const_cast<char*>(data), (int)length, 0);
                    DeSerializer<thrift_deserialize_adapter, Tuple> dr(std::forward<Tuple>(tuple), gos);

                    return dr.template GetTuple<T>();
                }
                catch (std::exception& e)
                {
                    throw cytx::net_exception(cytx::error_code::codec_fail, e.what());
                }
            }

            template<typename T>
            T unpack(GameObjectStream& gos)
            {
                try
                {
                    DeSerializer<thrift_deserialize_adapter> dr(gos);

                    T t{};
                    dr.DeSerialize(t);
                    return t;
                }
                catch (std::exception& e)
                {
                    throw cytx::net_exception(cytx::error_code::codec_fail, e.what());
                }
            }

            template<typename T, typename Tuple>
            T unpack(GameObjectStream& gos, Tuple&& tuple)
            {
                try
                {
                    DeSerializer<thrift_deserialize_adapter, Tuple> dr(std::forward<Tuple>(tuple), gos);

                    return dr.template GetTuple<T>();
                }
                catch (std::exception& e)
                {
                    throw cytx::net_exception(cytx::error_code::codec_fail, e.what());
                }
            }

            using buffer_type = gos_buffer;

            template <typename ... Args>
            buffer_type pack_args(Args&& ... args) const
            {
                auto args_tuple = std::make_tuple(std::forward<Args>(args)...);
                return pack(std::move(args_tuple));
            }

            template <typename T>
            buffer_type pack(T&& t) const
            {
                try
                {
                    Serializer<thrift_serialize_adapter> sr;
                    GameObjectStream gos = sr.Serialize(std::forward<T>(t));
                    buffer_type buffer(gos);
                    return std::move(buffer);
                }
                catch (std::exception& e)
                {
                    throw cytx::net_exception(cytx::error_code::codec_fail, e.what());
                }
            }

            template <typename ... Args>
            void pack_args(GameObjectStream& gos, Args&& ... args) const
            {
                auto args_tuple = std::make_tuple(std::forward<Args>(args)...);
                pack(gos, std::move(args_tuple));
            }

            template <typename T>
            void pack(GameObjectStream& gos, T&& t) const
            {
                try
                {
                    Serializer<thrift_serialize_adapter> sr;
                    sr.Serialize(gos, std::forward<T>(t));
                }
                catch (std::exception& e)
                {
                    throw cytx::net_exception(cytx::error_code::codec_fail, e.what());
                }
            }
        };
} }