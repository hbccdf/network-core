#pragma once
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include "raw_msg.hpp"
#include "msg_pack.hpp"

namespace cytx
{
    namespace gameserver
    {
        namespace detail
        {
            template <typename CompletionToken, typename Signature>
            using handler_type_t = typename boost::asio::handler_type<
                CompletionToken, Signature>::type;

            template <typename Handler>
            using async_result = boost::asio::async_result<Handler>;

            template <typename Signature, typename CompletionToken>
            using async_result_t = async_result<handler_type_t<CompletionToken, Signature>>;


            /// @brief Helper type used to initialize the asnyc_result with the handler.
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

            using yield_t = boost::asio::yield_context;

            template<typename T>
            using handler_t = handler_type_t<yield_t, void(T)>;

            template<typename T, typename FUNC>
            T async_await(boost::asio::io_service& ios, FUNC&& func)
            {
                using completion_t = async_completion<yield_t, void(T)>;

                T t;
                boost::asio::spawn(ios, [f = std::forward<FUNC>(func), &t](boost::asio::yield_context ctx)
                {
                    completion_t completion(std::forward<yield_t>(ctx));

                    func(completion.handler);
                    t = completion.result.get();
                });
                return t;
            }


            /* template<typename MSG>
             class call_manager
             {
                 using this_t = call_manager;
                 using yield_t = boost::asio::yield_context;
                 using io_service_t = boost::asio::io_service;
                 using completion_t = async_completion<yield_t, void(msg_ptr)>;
                 using handler_t = typename completion_t::handler_t;
                 using msg_ptr = std::shared_ptr<MSG>;
             public:
                 call_manager(io_service_t& ios)
                     : ios_(ios)
                 {
                 }

                 msg_ptr async_await_msg(msg_ptr send_msgp)
                 {
                     int32_t call_id = send_msgp->header().call_id;
                     if (call_id <= 0)
                     {
                         call_id = ++cur_call_id_;
                     }

                     msg_ptr msgp = nullptr;
                     auto& ios = get_current_ios();
                     boost::asio::spawn(ios, [&msgp, this, call_id](yield_t ctx)
                     {
                         completion_t completion(std::forward<yield_t>(ctx));

                         ios_.dispatch(std::bind(&this_t::register_call, this, call_id, completion.handler));
                         msgp = async_func(completion);
                     });
                     return msgp;
                 }

                 void notify_msg(msg_ptr msgp)
                 {
                     int32_t call_id = msgp->header().call_id;
                     auto it = calls_.find(call_id);
                     if (it == calls_.end())
                         return;

                     handler_t handler = it->second;

                     using boost::asio::asio_handler_invoke;
                     asio_handler_invoke(std::bind(handler, msgp), &handler);
                 }

             private:
                 void register_call(int32_t call_id, handler_t handler)
                 {
                     std::lock_guard<std::mutex> lock(call_mutex_);
                     calls_.emplace(call_id, handler);
                 }

                 msg_ptr async_func(completion_t& completion)
                 {
                     return completion.result.get();
                 }
             private:
                 io_service_t& ios_;
                 std::map<int32_t, handler_t> calls_;
                 std::atomic<uint32_t> cur_call_id_{ 1 };
             };*/
        }

        /*template<typename T>
        using call_manager = detail::call_manager<T>;*/
    }
}