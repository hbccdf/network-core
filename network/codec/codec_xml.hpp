﻿#pragma once
#include "network/traits/traits.hpp"
#include "network/serialize/serializer.hpp"
#include "network/serialize/deserializer.hpp"
#include "network/serialize/xml_adapter.hpp"

namespace cytx {
    namespace codec {
        struct xml_codec
        {
            xml_codec(bool) {}

            template<typename T>
            T unpack(const char* data, size_t length)
            {
                try
                {
                    DeSerializer<xml_deserialize_adapter> dr;
                    dr.parse(data, length);

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
            T unpack(const char* data, size_t length, Tuple&& tuple)
            {
                try
                {
                    DeSerializer<xml_deserialize_adapter, Tuple> dr(std::forward<Tuple>(tuple));
                    dr.parse(data, length);

                    return dr.template GetTuple<T>();
                }
                catch (std::exception& e)
                {
                    throw cytx::net_exception(error_code::codec_fail, e.what());
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
                    Serializer<xml_serialize_adapter> sr;
                    sr.Serialize(std::forward<T>(t));
                    return sr.get_adapter().str();
                }
                catch (std::exception& e)
                {
                    throw cytx::net_exception(error_code::codec_fail, e.what());
                }
            }
        };
} }