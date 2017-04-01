#pragma once
#include "../base/GameObjectStream.h"
#include <fmt/format.h>
#include "../base/utils.hpp"
#include "serializer.hpp"
#include "deserializer.hpp"
#include <thrift/transport/TTransportException.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/TBase.h>

#include <boost/make_shared.hpp>
#include <boost/smart_ptr/enable_shared_from_this.hpp>

namespace cytx
{
    using namespace cytx::rpc;
    using namespace apache::thrift;
    using namespace apache::thrift::protocol;
    using namespace apache::thrift::transport;

    struct thrift_serialize_adapter {};
    struct thrift_deserialize_adapter {};

    template<>
    class Serializer<thrift_serialize_adapter> : boost::noncopyable
    {
        using adapter_t = GameObjectStream;
    public:
        Serializer()
        {
        }

        ~Serializer()
        {
        }

        template<typename T>
        auto Serialize(T& t) -> std::enable_if_t<is_tuple<T>::value, GameObjectStream>
        {
            auto mem_buf = boost::make_shared<TMemoryBuffer>();
            auto compct_proto = boost::make_shared<TCompactProtocol>(mem_buf);
            std::get<0>(t).write(compct_proto.get());

            uint32_t length = 0;
            uint8_t* body_buf_ptr = nullptr;
            mem_buf->getBuffer(&body_buf_ptr, &length);

            GameObjectStream gos(length);
            gos.pushBinary((char*)body_buf_ptr, length);
            return gos;
        }

        template<typename T>
        auto Serialize(T& t) -> std::enable_if_t<!is_tuple<T>::value, GameObjectStream>
        {
            auto mem_buf = boost::make_shared<TMemoryBuffer>();
            auto compct_proto = boost::make_shared<TCompactProtocol>(mem_buf);
            t.write(compct_proto.get());

            uint32_t length = 0;
            uint8_t* body_buf_ptr = nullptr;
            mem_buf->getBuffer(&body_buf_ptr, &length);

            GameObjectStream gos(length);
            gos.pushBinary((char*)body_buf_ptr, length);
            return gos;
        }
    };

    template<typename OtherTuple>
    class DeSerializer<thrift_deserialize_adapter, OtherTuple> : boost::noncopyable
    {
        typedef GameObjectStream adapter_t;
    public:
        DeSerializer(adapter_t& gos)
            : gos_(gos)
        {
        }

        DeSerializer(OtherTuple&& t, adapter_t& gos)
            : tuple(std::move(t))
            , gos_(gos)
        {
        }

        ~DeSerializer()
        {
        }

        void set_tuple(OtherTuple&& t)
        {
            tuple = std::move(t);
        }

        adapter_t& get_adapter() { return gos_; }

        template<typename T>
        void DeSerialize(T& t)
        {
            ReadObject(t);
        }

        template<typename T>
        auto GetTuple()->std::enable_if_t < 0 < std::tuple_size<T>::value, T >
        {
            return make<T, 0>(std::true_type{}, std::tuple<>{});
        }

        template<typename T>
        auto GetTuple()->std::enable_if_t < 0 == std::tuple_size<T>::value, T >
        {
            return std::tuple<>{};
        }

    private:
        template <typename T, size_t Is, typename ReturnT>
        inline auto make(std::true_type, ReturnT&& t)
        {
            using value_type = std::conditional_t<(Is + 1) == std::tuple_size<T>::value, std::false_type, std::true_type>;
            using elem_t = std::tuple_element_t<Is, T>;
            return make<T, Is + 1>(value_type{}, std::tuple_cat(t, std::tuple<elem_t>(get_tuple_elem<elem_t>())));
        }

        template <typename T, size_t Is, typename ReturnT>
        inline auto make(std::false_type, ReturnT&& t)
        {
            return t;
        }

        template<typename T>
        auto get_tuple_elem() -> std::enable_if_t<tuple_contains<T, OtherTuple>::value, T>
        {
            return std::get<tuple_index<T, OtherTuple>::value>(tuple);
        }

        template<typename T>
        auto get_tuple_elem() -> std::enable_if_t<!tuple_contains<T, OtherTuple>::value, T>
        {
            T t;
            ReadObject(t);
            return std::move(t);
        }

        template<typename T>
        auto ReadObject(T& t)
        {
            auto membuffer = boost::make_shared<TMemoryBuffer>();
            auto protocol = boost::make_shared<TCompactProtocol>(membuffer);

            membuffer->resetBuffer((uint8_t*)gos_.data_, gos_.length());
            t.read(protocol.get());
        }
    private:
        GameObjectStream& gos_;
        OtherTuple tuple;
    };
}