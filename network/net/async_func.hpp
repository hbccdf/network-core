#pragma once
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include "raw_msg.hpp"

namespace cytx
{
    namespace net
    {
        namespace detail
        {
            template <typename CompletionToken, typename Signature>
            using handler_type_t = typename boost::asio::handler_type<CompletionToken, Signature>::type;

            template <typename Handler>
            using async_result = boost::asio::async_result<Handler>;

            template <typename Signature, typename CompletionToken>
            using async_result_t = async_result<handler_type_t<CompletionToken, Signature>>;


            //Helper type used to initialize the asnyc_result with the handler.
            template <typename CompletionToken, typename Signature>
            struct async_completion
            {
                using handler_t = handler_type_t<CompletionToken, Signature>;

                async_completion(CompletionToken&& token)
                    : handler(std::forward<CompletionToken>(token)),
                    result(handler)
                {}

                handler_t handler;
                async_result<handler_t> result;
            };
        }

        using yield_t = boost::asio::yield_context;
        using context_t = yield_t;

        template<typename T>
        using handler_t = detail::handler_type_t<yield_t, void(T)>;

        template<typename T>
        using completion_t = detail::async_completion<yield_t, void(T)>;
    }
}