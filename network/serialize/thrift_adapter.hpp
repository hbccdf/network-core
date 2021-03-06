#pragma once
#include <fmt/format.h>
#include "network/base/memory_stream.hpp"
#include "network/util/net.hpp"
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
    using namespace apache::thrift;
    using namespace apache::thrift::protocol;
    using namespace apache::thrift::transport;

    struct thrift_serialize_adapter {};
    struct thrift_deserialize_adapter {};

    template<>
    class Serializer<thrift_serialize_adapter>
    {
        using adapter_t = memory_stream;
    public:
        Serializer()
        {
        }

        ~Serializer()
        {
        }

        template<typename T>
        auto Serialize(T& t) -> std::enable_if_t<is_tuple_v<T>, memory_stream>
        {
            auto mem_buf = boost::make_shared<TMemoryBuffer>();
            auto compct_proto = boost::make_shared<TCompactProtocol>(mem_buf);
            std::get<0>(t).write(compct_proto.get());

            uint32_t length = 0;
            uint8_t* body_buf_ptr = nullptr;
            mem_buf->getBuffer(&body_buf_ptr, &length);

            memory_stream gos(length);
            gos.write_binary((char*)body_buf_ptr, length);
            return gos;
        }

        template<typename T>
        auto Serialize(T& t) -> std::enable_if_t<!is_tuple_v<T>, memory_stream>
        {
            auto mem_buf = boost::make_shared<TMemoryBuffer>();
            auto compct_proto = boost::make_shared<TCompactProtocol>(mem_buf);
            t.write(compct_proto.get());

            uint32_t length = 0;
            uint8_t* body_buf_ptr = nullptr;
            mem_buf->getBuffer(&body_buf_ptr, &length);

            memory_stream gos(length);
            gos.write_binary((char*)body_buf_ptr, length);
            return gos;
        }

        template<typename T>
        auto Serialize(memory_stream& gos, T& t) -> std::enable_if_t<is_tuple_v<T>>
        {
            auto mem_buf = boost::make_shared<TMemoryBuffer>();
            auto compct_proto = boost::make_shared<TCompactProtocol>(mem_buf);
            std::get<0>(t).write(compct_proto.get());

            uint32_t length = 0;
            uint8_t* body_buf_ptr = nullptr;
            mem_buf->getBuffer(&body_buf_ptr, &length);

            gos.write_binary((char*)body_buf_ptr, length);
        }

        template<typename T>
        auto Serialize(memory_stream& gos, T& t) -> std::enable_if_t<!is_tuple_v<T>>
        {
            auto mem_buf = boost::make_shared<TMemoryBuffer>();
            auto compct_proto = boost::make_shared<TCompactProtocol>(mem_buf);
            t.write(compct_proto.get());

            uint32_t length = 0;
            uint8_t* body_buf_ptr = nullptr;
            mem_buf->getBuffer(&body_buf_ptr, &length);

            gos.write_binary((char*)body_buf_ptr, length);
        }
    };

    template<typename OtherTuple>
    class DeSerializer<thrift_deserialize_adapter, OtherTuple> : public BaseDeSerializer<thrift_deserialize_adapter, OtherTuple>
    {
        using base_t = BaseDeSerializer<thrift_deserialize_adapter, OtherTuple>;
        using gos_t = memory_stream;
    public:
        DeSerializer(gos_t& gos)
            : base_t()
            , gos_(gos)
        {
        }

        DeSerializer(OtherTuple&& t, gos_t& gos)
            : base_t(std::forward<OtherTuple>(t))
            , gos_(gos)
        {
        }

        ~DeSerializer()
        {
        }

        gos_t& get_gos() { return gos_; }

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
        auto make(std::true_type, ReturnT&& t)
        {
            using value_type = std::conditional_t<(Is + 1) == std::tuple_size<T>::value, std::false_type, std::true_type>;
            using elem_t = std::tuple_element_t<Is, T>;
            return make<T, Is + 1>(value_type{}, std::tuple_cat(t, std::tuple<elem_t>(get_tuple_elem<elem_t>())));
        }

        template <typename T, size_t Is, typename ReturnT>
        auto make(std::false_type, ReturnT&& t)
        {
            return t;
        }

        template<typename T>
        auto get_tuple_elem() -> std::enable_if_t<tuple_contains<T, OtherTuple>::value, T>
        {
            return std::get<tuple_index<T, OtherTuple>::value>(tuple_);
        }

        template<typename T>
        auto get_tuple_elem() -> std::enable_if_t<!tuple_contains<T, OtherTuple>::value, T>
        {
            T t{};
            ReadObject(t);
            return std::move(t);
        }

        template<typename T>
        auto ReadObject(T& t)
        {
            auto membuffer = boost::make_shared<TMemoryBuffer>();
            auto protocol = boost::make_shared<TCompactProtocol>(membuffer);

            membuffer->resetBuffer((uint8_t*)gos_.rd_ptr(), gos_.length());
            t.read(protocol.get());
        }
    private:
        memory_stream& gos_;
    };
}