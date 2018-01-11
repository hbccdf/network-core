#pragma once
//#include <boost/asio.hpp>
//#include <boost/asio/spawn.hpp>
//#include "raw_msg.hpp"
//#include "msg_pack.hpp"
//
//namespace cytx
//{
//    namespace gameserver
//    {
//        namespace detail
//        {
//            template <typename CompletionToken, typename Signature>
//            using handler_type_t = typename boost::asio::handler_type<
//                CompletionToken, Signature>::type;
//
//            template <typename Handler>
//            using async_result = boost::asio::async_result<Handler>;
//
//            template <typename Signature, typename CompletionToken>
//            using async_result_t = async_result<handler_type_t<CompletionToken, Signature>>;
//
//
//            /// @brief Helper type used to initialize the asnyc_result with the handler.
//            template <typename CompletionToken, typename Signature>
//            struct async_completion
//            {
//                using handler_t = handler_type_t<CompletionToken, Signature>;
//
//                async_completion(CompletionToken&& token)
//                    : handler(std::forward<CompletionToken>(token)),
//                    result(handler)
//                {}
//
//                handler_t handler;
//                async_result<handler_t> result;
//            };
//
//            using yield_t = boost::asio::yield_context;
//
//            template<typename T>
//            using handler_t = handler_type_t<yield_t, void(T)>;
//
//            template<typename T, typename FUNC>
//            T async_await(boost::asio::io_service& ios, FUNC&& func)
//            {
//                using completion_t = async_completion<yield_t, void(T)>;
//
//                T t;
//                boost::asio::spawn(ios, [f = std::forward<FUNC>(func), &t](boost::asio::yield_context ctx)
//                {
//                    completion_t completion(std::forward<yield_t>(ctx));
//
//                    f(completion.handler);
//                    t = completion.result.get();
//                });
//                return t;
//            }
//
//            template<typename T, typename FUNC, typename HANDLER>
//            void async_await(boost::asio::io_service& ios, FUNC&& func, HANDLER&& handler)
//            {
//                using completion_t = async_completion<yield_t, void(T)>;
//
//                boost::asio::spawn(ios, [f = std::forward<FUNC>(func), h = std::forward<HANDLER>(handler)](boost::asio::yield_context ctx)
//                {
//                    completion_t completion(std::forward<yield_t>(ctx));
//
//                    f(completion.handler);
//                    T t = completion.result.get();
//                    h(t);
//                });
//            }
//        }
//    }
//}