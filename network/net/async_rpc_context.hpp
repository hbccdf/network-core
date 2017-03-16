#pragma once
#include <functional>
#include <vector>
#include <boost/asio.hpp>
#include <boost/make_unique.hpp>
#include "../base/excetion.hpp"
#include "wait_barrier.hpp"
#include <queue>

namespace cytx {
    namespace rpc
    {
        template<typename buffer_type, typename header_type>
        struct get_context;

        template <typename buffer_type, typename header_type>
        struct base_rpc_context
        {
            using buffer_t = buffer_type;
            using success_function_t = std::function<void(char const*, size_t)>;
            using on_error_function_t = std::function<void(exception const&)>;
            using asio_buffers = std::vector<boost::asio::const_buffer>;
            using post_func_t = std::function<void()>;
            using message_t = std::vector<char>;
            using result_barrier_t = ios_result_barrier;
            using context_t = typename get_context<buffer_type, header_type>::type;
            using header_t = header_type;
            using ios_t = ios_wrapper;

        public:
            base_rpc_context(ios_t& ios, header_t const& h, buffer_t&& msg)
                : ios_(ios)
                , send_msg(std::move(msg))
                , head(h)
                , send_buffer({
                boost::asio::buffer(&head, sizeof(head)),
                boost::asio::buffer(send_msg.data(), send_msg.size()) })
            {
            }
        public:
            base_rpc_context(ios_t& ios, uint32_t protocol, buffer_t&& msg)
                : ios_(ios)
                , send_msg(std::move(msg))
                , head(0, protocol, static_cast<uint32_t>(send_msg.size()))
                , send_buffer({
                boost::asio::buffer(&head, sizeof(head)),
                boost::asio::buffer(send_msg.data(), send_msg.size()) })
            {
            }

            // for reponse
            base_rpc_context(ios_t& ios, header_t const& h, buffer_t&& msg, post_func_t postf)
                : ios_(ios)
                , head(h)
                , send_msg(std::move(msg))
                , post_func(std::move(postf))
                , send_buffer({
                boost::asio::buffer(&head, sizeof(head)),
                boost::asio::buffer(send_msg.data(), send_msg.size()) })
            {
                head.reply(true);
                head.length(static_cast<uint32_t>(send_msg.size()));
            }

            base_rpc_context(ios_t& ios, buffer_t&& msg, post_func_t postf)
                : ios_(ios)
                , send_msg(std::move(msg))
                , post_func(std::move(postf))
                , send_buffer({
                boost::asio::buffer(&head, sizeof(head)),
                boost::asio::buffer(send_msg) })
            {
                head.reply(true);
                head.length(static_cast<uint32_t>(send_msg.size()));
            }

            base_rpc_context(ios_t& ios, header_t const& h, buffer_t& msg, post_func_t postf)
                : base_rpc_context(ios, h, std::move(buffer_t{ msg.begin(), msg.end() }), std::move(postf))
            {
            }

            base_rpc_context(ios_t& ios, buffer_t& msg, post_func_t postf)
                : base_rpc_context(ios, std::move(buffer_t{ msg.begin(), msg.end() }), std::move(postf))
            {
            }

            void ntoh()
            {
                head.ntoh();
            }

            void hton()
            {
                head.hton();
            }

            header_t& get_head()
            {
                return head;
            }

            bool is_reply() const
            {
                return head.is_reply();
            }

            bool need_reply() const
            {
                return head.need_reply();
            }

            decltype(auto) get_send_message() const
            {
                return send_buffer;
            }

            auto get_recv_message(size_t size)
            {
                recv_msg.resize(size);
                return boost::asio::buffer(recv_msg);
            }

            void ok()
            {
                if (!is_over)
                {
                    is_over = true;

                    if (on_ok)
                        on_ok(recv_msg.data(), recv_msg.size());

                    if (nullptr != barrier_ptr)
                        barrier_ptr->notify();
                }
            }

            void error(exception&& recv_error)
            {
                err = std::move(recv_error);

                post_error();
            }

            void error(error_code errcode, std::string const& message = "")
            {
                err.set_code(errcode);
                if (!message.empty())
                {
                    err.set_message(message);
                }

                post_error();
            }

            void post_error()
            {
                if (!is_over)
                {
                    is_over = true;

                    if (on_error)
                        on_error(err);

                    if (nullptr != barrier_ptr)
                        barrier_ptr->notify();
                }
            }

            void create_barrier()
            {
                if (nullptr == barrier_ptr)
                    barrier_ptr.reset(new ios_result_barrier(ios_));
            }

            void wait()
            {
                if (nullptr != barrier_ptr)
                    barrier_ptr->wait();

                if (err)
                    throw err;
            }

            void apply_post_func() const
            {
                if (post_func)
                    post_func();
            }

            // for response
            static auto make_error_message(ios_t& ios, header_t const& h, buffer_t&& msg, post_func_t postf = nullptr)
            {
                auto ctx = make_message(ios, h, std::forward<buffer_t>(msg), std::move(postf));
                ctx->head.result(static_cast<uint16_t>(result_code::fail));
                return ctx;
            }

            static auto make_message(ios_t& ios, header_t const& h, buffer_t&& msg, post_func_t postf = nullptr)
            {
                return std::make_shared<context_t>(ios, h, std::forward<buffer_t>(msg), std::move(postf));
            }

            static auto make_message(ios_t& ios, buffer_t&& msg, post_func_t postf = nullptr)
            {
                return std::make_shared<context_t>(ios, std::forward<buffer_t>(msg), std::move(postf));
            }

            static auto make_error_message(ios_t& ios, header_t const& h, buffer_t& msg, post_func_t postf = nullptr)
            {
                auto ctx = make_message(ios, h, msg, std::move(postf));
                ctx->head.result(static_cast<uint16_t>(result_code::fail));
                return ctx;
            }

            static auto make_message(ios_t& ios, header_t const& h, buffer_t& msg, post_func_t postf = nullptr)
            {
                return std::make_shared<context_t>(ios, h, msg, std::move(postf));
            }

            static auto make_message(ios_t& ios, buffer_t& msg, post_func_t postf = nullptr)
            {
                return std::make_shared<context_t>(ios, msg, std::move(postf));
            }

            ios_t& ios_;
            buffer_t send_msg;
            header_t head;
            std::vector<char> recv_msg;
            asio_buffers send_buffer;
            exception err;
            success_function_t on_ok;
            on_error_function_t on_error;
            post_func_t post_func;
            std::unique_ptr<result_barrier_t> barrier_ptr;
            bool is_over = false;
        };

        template<typename buffer_type>
        using client_rpc_context = base_rpc_context<buffer_type, client_msg_header>;

        template <typename buffer_type>
        struct server_rpc_context : public base_rpc_context<buffer_type, msg_header>
        {
            using base_t = base_rpc_context<buffer_type, msg_header>;
            using buffer_t = buffer_type;
            using success_function_t = std::function<void(char const*, size_t)>;
            using on_error_function_t = std::function<void(exception const&)>;
            using asio_buffers = std::vector<boost::asio::const_buffer>;
            using post_func_t = std::function<void()>;
            using message_t = std::vector<char>;
            using result_barrier_t = ios_result_barrier;
            using context_t = server_rpc_context<buffer_t>;
            using header_t = msg_header;

            using base_t::base_t;
            // for call
            server_rpc_context(ios_t& ios, msg_way way, uint64_t hash, buffer_t&& msg)
                : base_t(ios, header_t{ 0,{ false, way == msg_way::one_way }, 0, (uint32_t)(msg.size()), hash },
                    std::move(msg))
            {
            }
        };

        template<typename buffer_type, typename header_type>
        struct get_context
        {
            using type = base_rpc_context<buffer_type, header_type>;
        };

        template<typename buffer_type>
        struct get_context<buffer_type, msg_header>
        {
            using type = server_rpc_context<buffer_type>;
        };

        template<typename buffer_type, typename header_type>
        using get_context_t = typename get_context<buffer_type, header_type>::type;

        template <typename CodecPolicy, typename header_type>
        class base_call_container
        {
        public:
            using codec_policy = CodecPolicy;
            using context_t = get_context_t<typename codec_policy::buffer_type, header_type>;
            using context_ptr = std::shared_ptr<context_t>;
            using call_map_t = std::map<uint32_t, context_ptr>;
            using call_list_t = std::list<context_ptr>;

        public:
            explicit base_call_container()
                : call_id_(0)
            {
            }

            bool push_call(context_ptr& ctx)
            {
                if(!ctx->is_reply())
                    push_call_response(ctx);
                call_list_.push_back(ctx);
                return true;
            }

            void push_call_response(context_ptr& ctx)
            {
                if (ctx->get_head().need_reply())
                {
                    auto call_id = ++call_id_;
                    ctx->get_head().call_id(call_id);
                    call_map_.emplace(call_id, ctx);
                }
            }

            void task_calls_from_list(call_list_t& to_calls)
            {
                to_calls = std::move(call_list_);
            }

            bool call_list_empty() const
            {
                return call_list_.empty();
            }

            context_ptr get_call_from_map(uint32_t call_id)
            {
                auto itr = call_map_.find(call_id);
                if (call_map_.end() != itr)
                {
                    context_ptr ctx = itr->second;
                    call_map_.erase(itr);
                    return ctx;
                }
                return nullptr;
            }

            void remove_call_from_map(uint32_t call_id)
            {
                auto itr = call_map_.find(call_id);
                if (call_map_.end() != itr)
                    call_map_.erase(itr);
            }

            void task_calls_from_map(call_map_t& call_map)
            {
                call_map = std::move(call_map_);
            }

            size_t get_call_list_size() const
            {
                return call_list_.size();
            }

            size_t get_call_map_size() const
            {
                return call_map_.size();
            }

        private:
            call_map_t  call_map_;
            call_list_t call_list_;
            uint32_t    call_id_;
        };

        template <typename CodecPolicy>
        class base_call_container<CodecPolicy, client_msg_header>
        {
        public:
            using codec_policy = CodecPolicy;
            using header_t = client_msg_header;
            using context_t = get_context_t<typename codec_policy::buffer_type, header_t>;
            using context_ptr = std::shared_ptr<context_t>;
            using call_map_t = std::map<uint32_t, std::queue<context_ptr>>;
            using call_list_t = std::list<context_ptr>;

        public:
            explicit base_call_container()
            {
            }

            bool push_call(context_ptr& ctx)
            {
                if (!ctx->is_reply())
                    push_call_response(ctx);
                call_list_.push_back(ctx);
                return true;
            }

            void push_call_response(context_ptr& ctx)
            {
                if (ctx->get_head().need_reply())
                {
                    auto protocol_id = reply_protocol(ctx->get_head().protocol_id);
                    if (call_map_.find(protocol_id) != call_map_.end())
                    {
                        call_map_[protocol_id].push(ctx);
                    }
                    else
                    {
                        std::queue<context_ptr> q;
                        q.push(ctx);
                        call_map_.emplace(protocol_id, std::move(q));
                    }
                }
            }

            void task_calls_from_list(call_list_t& to_calls)
            {
                to_calls = std::move(call_list_);
            }

            bool call_list_empty() const
            {
                return call_list_.empty();
            }

            context_ptr get_call_from_map(uint32_t protocol_id)
            {
                auto itr = call_map_.find(protocol_id);
                if (call_map_.end() != itr)
                {
                    auto& q = itr->second;
                    context_ptr ctx = q.front();
                    q.pop();
                    if(q.empty())
                        call_map_.erase(itr);
                    return ctx;
                }
                return nullptr;
            }

            void remove_call_from_map(uint32_t protocol_id)
            {
                auto itr = call_map_.find(call_id);
                if (call_map_.end() != itr)
                {
                    itr->second.pop();
                    if (itr->second.empty())
                        call_map_.erase(itr);
                }
            }

            void task_calls_from_map(call_map_t& call_map)
            {
                call_map = std::move(call_map_);
            }

            size_t get_call_list_size() const
            {
                return call_list_.size();
            }

            size_t get_call_map_size() const
            {
                return call_map_.size();
            }

        private:
            call_map_t  call_map_;
            call_list_t call_list_;
            uint32_t    call_id_;
        };

        template<typename CodecPolicy>
        using client_rpc_container = base_call_container<CodecPolicy, client_msg_header>;

        template<typename CodecPolicy>
        using server_rpc_container = base_call_container<CodecPolicy, msg_header>;
    }
}