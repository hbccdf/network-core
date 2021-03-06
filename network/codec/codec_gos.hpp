#pragma once
#include "network/serialize/serializer.hpp"
#include "network/serialize/deserializer.hpp"
#include "network/serialize/gos_adapter.hpp"
#include "network/net/exception.hpp"
#include "codec_common.hpp"

namespace cytx {
    namespace codec {
        struct gos_codec
        {
            using buffer_type = gos_buffer;

            gos_codec(bool big_endian) : is_big_endian_(big_endian) {}

            template<typename T>
            T unpack(char const* data, size_t length)
            {
                try
                {
                    memory_stream gos(const_cast<char*>(data), (int)length, 0);
                    DeSerializer<gos_deserialize_adapter> dr(gos, is_big_endian_);

                    T t{};
                    dr.DeSerialize(t);
                    return t;
                }
                catch (std::exception& e)
                {
                    throw cytx::net_exception(error_code::codec_fail, e.what());
                }
            }

            template<typename T, typename Tuple>
            T unpack(char const* data, size_t length, Tuple&& tuple)
            {
                try
                {
                    memory_stream gos(const_cast<char*>(data), (int)length, 0);
                    DeSerializer<gos_deserialize_adapter, Tuple> dr(std::forward<Tuple>(tuple), gos, is_big_endian_);

                    return dr.template GetTuple<T>();
                }
                catch (std::exception& e)
                {
                    throw cytx::net_exception(error_code::codec_fail, e.what());
                }
            }

            template<typename T>
            T unpack(memory_stream& gos)
            {
                try
                {
                    DeSerializer<gos_deserialize_adapter> dr(gos, is_big_endian_);

                    T t{};
                    dr.DeSerialize(t);
                    return t;
                }
                catch (std::exception& e)
                {
                    throw cytx::net_exception(error_code::codec_fail, e.what());
                }
            }

            template<typename T, typename Tuple>
            T unpack(memory_stream& gos, Tuple&& tuple)
            {
                try
                {
                    DeSerializer<gos_deserialize_adapter, Tuple> dr(std::forward<Tuple>(tuple), gos, is_big_endian_);

                    return dr.template GetTuple<T>();
                }
                catch (std::exception& e)
                {
                    throw cytx::net_exception(error_code::codec_fail, e.what());
                }
            }

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
                    int total_size = tuple_total_size<T>::value;
                    if (total_size == 0)
                    {
                        //�հ�
                        return buffer_type{};
                    }
                    if (total_size < 0)
                    {
                        total_size = memory_stream::defualt_size();
                    }
                    memory_stream gos(total_size);
                    Serializer<gos_serialize_adapter> sr(gos, is_big_endian_);
                    sr.Serialize(std::forward<T>(t));
                    gos_buffer buffer(gos);
                    return std::move(buffer);
                }
                catch (std::exception& e)
                {
                    throw cytx::net_exception(error_code::codec_fail, e.what());
                }
            }

            template <typename ... Args>
            void pack_args(memory_stream& gos, Args&& ... args) const
            {
                auto args_tuple = std::make_tuple(std::forward<Args>(args)...);
                pack(gos, std::move(args_tuple));
            }

            template <typename T>
            void pack(memory_stream& gos, T&& t) const
            {
                try
                {
                    Serializer<gos_serialize_adapter> sr(gos, is_big_endian_);
                    sr.Serialize(std::forward<T>(t));
                }
                catch (std::exception& e)
                {
                    throw cytx::net_exception(error_code::codec_fail, e.what());
                }
            }

        private:
            bool is_big_endian_;
        };
} }