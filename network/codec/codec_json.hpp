#pragma once
#include "../traits/traits.hpp"
#include "../serialize/serializer.hpp"
#include "../serialize/deserializer.hpp"
#include "../base/excetion.hpp"

namespace cytx {
    namespace rpc {
        struct json_codec
        {
            json_codec(bool) {}

            template<typename T>
            T unpack(char const* data, size_t length)
            {
                try
                {
                    DeSerializer<json_deserialize_adapter> dr;
                    dr.parse(data, length);

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
                    DeSerializer<json_deserialize_adapter, Tuple> dr(std::forward<Tuple>(tuple));
                    dr.parse(data, length);

                    return dr.GetTuple<T>();
                }
                catch (std::exception& e)
                {
                    throw cytx::rpc::rpc_exception(error_code::codec_fail, e.what());
                }
            }

            using buffer_type = std::string;

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
                    Serializer<json_serialize_adapter> sr;
                    sr.Serialize(std::forward<T>(t));
                    return sr.get_adapter().str();
                }
                catch (std::exception& e)
                {
                    throw cytx::rpc::rpc_exception(error_code::codec_fail, e.what());
                }
            }
        };
} }