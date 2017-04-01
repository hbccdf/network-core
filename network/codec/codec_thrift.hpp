#pragma once
#include "../serialize/thrift_adapter.hpp"
#include "codec_common.hpp"

namespace cytx {
    namespace rpc {

        struct thrift_codec
        {
            thrift_codec(bool) {}

            template<typename T>
            T unpack(char const* data, size_t length)
            {
                try
                {
                    GameObjectStream gos(const_cast<char*>(data), (int)length, 0);
                    DeSerializer<thrift_deserialize_adapter> dr(gos);

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
                    DeSerializer<thrift_deserialize_adapter, Tuple> dr(std::forward<Tuple>(tuple), gos);

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
                    Serializer<thrift_serialize_adapter> sr;
                    GameObjectStream gos = sr.Serialize(std::forward<T>(t));
                    buffer_type buffer;
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
        };
} }