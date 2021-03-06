#pragma once
#include "router.hpp"
#include "invoker_traits.hpp"
namespace cytx
{
    namespace rpc
    {
        template<typename T>
        bool router_base<T>::register_invoker(uint64_t name, invoker_t&& invoker)
        {
            return invokers_.emplace(name, std::move(invoker)).second;
        }

        template<typename T>
        bool router_base<T>::has_invoker(uint64_t name) noexcept
        {
            invokers_.find(name) != invokers_.end();
        }

        template<typename T>
        void router_base<T>::on_read(connection_ptr const& conn_ptr, header_t& header)
        {
            if (on_read_)
                on_read_(conn_ptr, header);
        }

        template<typename T>
        void router_base<T>::on_error(connection_ptr const& conn_ptr, net_result const& error)
        {
            if (on_error_)
                on_error_(conn_ptr, error);
        }

        template<typename T>
        void router_base<T>::set_on_read(on_read_func&& on_read)
        {
            on_read_ = std::forward<on_read_func>(on_read);
        }

        template<typename T>
        void router_base<T>::set_on_error(on_error_func&& on_error)
        {
            on_error_ = std::forward<on_error_func>(on_error);
        }

        template<typename T>
        void router_base<T>::set_before_invoker(before_invoke_func&& before_invoker)
        {
            before_invoker_ = std::forward<before_invoke_func>(before_invoker);
        }

        template<typename T, typename H>
        template <typename Handler, typename PostFunc>
        bool router<T, H>::register_invoker(std::string const& name, Handler&& handler, PostFunc&& post_func)
        {
            return register_codec_invoker<codec_policy>(name, std::forward<Handler>(handler), std::forward<PostFunc>(post_func));
        }

        template<typename T, typename H> template <typename CallCodecPolicy, typename Handler, typename PostFunc>
        bool router<T, H>::register_codec_invoker(std::string const& name, Handler&& handler, PostFunc&& post_func)
        {
            return register_codec_invoker_impl<CallCodecPolicy, Handler>(hash_(name),
                std::forward<Handler>(handler), std::forward<PostFunc>(post_func));
        }

        template<typename T, typename H>
        template <typename Handler, typename PostFunc>
        bool router<T, H>::register_invoker(uint32_t protocol, Handler&& handler, PostFunc&& post_func)
        {
            return register_codec_invoker<codec_policy>(protocol, std::forward<Handler>(handler), std::forward<PostFunc>(post_func));
        }

        template<typename T, typename H> template <typename CallCodecPolicy, typename Handler, typename PostFunc>
        bool router<T, H>::register_codec_invoker(uint32_t protocol, Handler&& handler, PostFunc&& post_func)
        {
            return register_codec_invoker_impl<CallCodecPolicy, Handler>(protocol,
                std::forward<Handler>(handler), std::forward<PostFunc>(post_func));
        }

        template<typename T, typename H>
        template <typename Handler>
        bool router<T, H>::register_invoker(std::string const& name, Handler&& handler)
        {
            return register_codec_invoker<codec_policy>(name, std::forward<Handler>(handler));
        }

        template<typename T, typename H>
        template <typename CallCodecPolicy, typename Handler>
        bool router<T, H>::register_codec_invoker(std::string const& name, Handler&& handler)
        {
            return register_codec_invoker_impl<CallCodecPolicy, Handler>(hash_(name), std::forward<Handler>(handler));
        }

        template<typename T, typename H>
        template <typename Handler>
        bool router<T, H>::register_invoker(uint32_t protocol, Handler&& handler)
        {
            return register_codec_invoker<codec_policy>(protocol, std::forward<Handler>(handler));
        }

        template<typename T, typename H>
        template <typename CallCodecPolicy, typename Handler>
        bool router<T, H>::register_codec_invoker(uint32_t protocol, Handler&& handler)
        {
            return register_codec_invoker_impl<CallCodecPolicy, Handler>(protocol, std::forward<Handler>(handler));
        }

        template<typename T, typename H>
        template <typename Handler>
        bool router<T, H>::register_raw_invoker(std::string const& name, Handler&& handler)
        {
            return register_raw_invoker_impl(hash_(name), std::forward<Handler>(handler));
        }

        template<typename T, typename H>
        template <typename Handler>
        bool router<T, H>::register_raw_invoker(uint32_t protocol, Handler&& handler)
        {
            return register_raw_invoker_impl(protocol, std::forward<Handler>(handler));
        }

        template<typename T, typename H>
        bool router<T, H>::has_invoker(std::string const& name) const
        {
            auto name_hash = hash_(name);
            return base_t::has_invoker(name_hash);
        }

        template<typename T, typename H>
        void router<T, H>::apply_invoker(connection_ptr conn, msg_ptr msg) const
        {
            try
            {
                auto& header = msg->header();
                if (this->before_invoker_ && !this->before_invoker_(conn, msg))
                    return;

                for (auto& invoker : this->proto_invokers_)
                {
                    if (invoker.func(header))
                    {
                        invoker.invoker(conn, msg->header(), msg->data());
                        return;
                    }
                }

                auto itr = this->invokers_.find(header.proto());
                if (this->invokers_.end() == itr)
                {
                    if (conn->get_irouter())
                    {
                        conn->get_irouter()->on_receive(conn, msg);
                    }
                    else if (header.need_reply())
                    {
                        auto ctx = context_t::make_error_message(header, error_code::no_handler);
                        conn->response(ctx);
                    }
                }
                else
                {
                    auto& invoker = itr->second;
                    if (!invoker)
                    {
                        if (!header.need_reply())
                            return;
                        auto ctx = context_t::make_error_message(header, error_code::no_handler);
                        conn->response(ctx);
                        return;
                    }

                    invoker(conn, msg->header(), msg->data());
                }
            }
            catch (net_exception const& error)
            {
                if (!msg->header().need_reply())
                    return;
                // response serialized exception to client
                auto ctx = context_t::make_error_message(msg->header(), error.code());
                conn->response(ctx);
            }
        }

        template<typename T, typename H>
        template <typename CallCodecPolicy, typename Handler, typename ... Handlers>
        bool router<T, H>::register_codec_invoker_impl(uint64_t proto_hash, Handlers&& ... handlers)
        {
            auto itr = this->invokers_.find(proto_hash);
            if (this->invokers_.end() != itr)
                return false;

            using handler_traits_type = handler_traits<Handler>;
            using invoker_traits_type = invoker_traits<typename handler_traits_type::return_tag>;

            auto invoker = invoker_traits_type::template get<header_t, codec_policy, CallCodecPolicy>(std::forward<Handlers>(handlers)...);
            this->invokers_.emplace(proto_hash, std::move(invoker));
            return true;
        }

        template<typename T, typename H>
        template <typename Handler>
        bool router<T, H>::register_raw_invoker_impl(uint64_t proto_hash, Handler&& handler)
        {
            auto itr = this->invokers_.find(proto_hash);
            if (this->invokers_.end() != itr)
                return false;

            invoker_t invoker = std::forward<Handler>(handler);
            this->invokers_.emplace(proto_hash, std::move(invoker));
            return true;
        }

        template<typename T, typename H>
        template <typename Handler>
        bool router<T, H>::register_invoker(proto_func func, Handler&& handler)
        {
            invoker_t invoker = std::forward<Handler>(handler);
            this->proto_invokers_.emplace(typename base_t::proto_func_invoker{ func, std::move(invoker) });
            return true;
        }
    }
}