#pragma once
#include "../serialize/serializer.hpp"
#include "../serialize/deserializer.hpp"
#include "../serialize/gos_adapter.hpp"
#include "codec_common.hpp"

namespace cytx {
    namespace rpc {
        struct gos_codec
        {
            gos_codec(bool big_endian) : is_big_endian_(big_endian) {}

            template<typename T>
            T unpack(char const* data, size_t length)
            {
                try
                {
                    GameObjectStream gos(const_cast<char*>(data), (int)length, 0);
                    DeSerializer<gos_deserialize_adapter> dr(gos, is_big_endian_);

                    T t;
                    dr.DeSerialize(t);
                    return t;
                }
                catch (std::exception& e)
                {
                    throw cytx::rpc::rpc_exception(error_code::codec_fail, e.what());
                }
            }

            template<typename T, typename Tuple>
            T unpack(char const* data, size_t length, Tuple&& tuple)
            {
                try
                {
                    GameObjectStream gos(const_cast<char*>(data), (int)length, 0);
                    DeSerializer<gos_deserialize_adapter, Tuple> dr(std::forward<Tuple>(tuple), gos, is_big_endian_);

                    return dr.GetTuple<T>();
                }
                catch (std::exception& e)
                {
                    throw cytx::rpc::rpc_exception(error_code::codec_fail, e.what());
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
                    int total_size = tuple_total_size<T>::value;
                    if (total_size == 0)
                    {
                        //¿Õ°ü
                        return buffer_type{};
                    }
                    if (total_size < 0)
                    {
                        total_size = GameObjectStream::default_data_size;
                    }
                    GameObjectStream gos(total_size);
                    Serializer<gos_serialize_adapter> sr(gos, is_big_endian_);
                    sr.Serialize(std::forward<T>(t));
                    gos_buffer buffer;
                    buffer.data_ = gos.data_;
                    buffer.size_ = gos.length();
                    gos.alloc_type_ = 0;
                    return std::move(buffer);
                }
                catch (std::exception& e)
                {
                    throw cytx::rpc::rpc_exception(error_code::codec_fail, e.what());
                }
            }

        private:
            bool is_big_endian_;
        };
} }