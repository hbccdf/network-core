#pragma once
#include <functional>
#include <vector>
#include <queue>
#include <boost/asio.hpp>
#include "network/net/exception.hpp"
#include "network/base/waitable_object.hpp"
#include "network/timer/schedule_timer.hpp"
#include "msg_header.hpp"

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
            using on_error_function_t = std::function<void(net_result const&)>;
            using asio_buffers = std::vector<boost::asio::const_buffer>;
            using post_func_t = std::function<void()>;
            using message_t = std::vector<char>;
            using result_barrier_t = waitable_object;
            using context_t = typename get_context<buffer_type, header_type>::type;
            using header_t = header_type;
            using timer_t = schedule_timer;
            using duration_t = typename timer_t::duration_t;
            using timer_ptr = std::shared_ptr<timer_t>;

        public:
            base_rpc_context(header_t const& h, buffer_t&& msg)
                : send_msg(std::move(msg))
                , head(h)
                , send_buffer({
                boost::asio::buffer(&head, sizeof(head)),
                boost::asio::buffer(send_msg.data(), send_msg.size()) })
            {
            }

            base_rpc_context(uint32_t protocol, buffer_t&& msg)
                : send_msg(std::move(msg))
                , head(0, protocol, static_cast<uint32_t>(send_msg.size()))
                , send_buffer({
                boost::asio::buffer(&head, sizeof(head)),
                boost::asio::buffer(send_msg.data(), send_msg.size()) })
            {
            }

            // for reponse
            base_rpc_context(header_t const& h, buffer_t&& msg, post_func_t postf)
                : head(h)
                , send_msg(std::move(msg))
                , post_func(std::move(postf))
                , send_buffer({
                boost::asio::buffer(&head, sizeof(head)),
                boost::asio::buffer(send_msg.data(), send_msg.size()) })
            {
                head.length(static_cast<uint32_t>(send_msg.size()));
            }

            base_rpc_context(buffer_t&& msg, post_func_t postf)
                : send_msg(std::move(msg))
                , post_func(std::move(postf))
                , send_buffer({
                boost::asio::buffer(&head, sizeof(head)),
                boost::asio::buffer(send_msg) })
            {
                head.reply(true);
                head.length(static_cast<uint32_t>(send_msg.size()));
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

            uint32_t reply_protocol()
            {
                if (reply_protocol_id != 0)
                    return reply_protocol_id;
                else
                    return cytx::reply_protocol(head.protocol_id);
            }

            bool have_reply_process() const
            {
                return on_ok || on_error || barrier_ptr || timer_ptr_;
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

            void error(const net_result& recv_error)
            {
                err = recv_error;
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
                    barrier_ptr.reset(new result_barrier_t());
            }

            void wait()
            {
                if (nullptr != barrier_ptr)
                    barrier_ptr->wait();

                if (err)
                    throw net_exception(err);
            }

            void apply_post_func() const
            {
                if (post_func)
                    post_func();
            }

            void timeout(const duration_t& duration)
            {
                duration_ = duration;
            }

            // for response
            static auto make_error_message(header_t& h, error_code code)
            {
                h.result((uint16_t)code);
                return make_message(h, buffer_t{}, nullptr);
            }

            static auto make_message(header_t const& h, buffer_t&& msg, post_func_t postf = nullptr)
            {
                return std::make_shared<context_t>(h, std::forward<buffer_t>(msg), std::move(postf));
            }

            static auto make_message(buffer_t&& msg, post_func_t postf = nullptr)
            {
                return std::make_shared<context_t>(std::forward<buffer_t>(msg), std::move(postf));
            }

            buffer_t send_msg;
            header_t head;
            std::vector<char> recv_msg;
            asio_buffers send_buffer;
            net_result err;
            success_function_t on_ok;
            on_error_function_t on_error;
            post_func_t post_func;
            std::unique_ptr<result_barrier_t> barrier_ptr;
            bool is_over = false;
            timer_ptr timer_ptr_;
            duration_t duration_{0};
            uint32_t reply_protocol_id = 0;
        };

        template<typename buffer_type>
        using client_rpc_context = base_rpc_context<buffer_type, client_msg_header>;

        template <typename buffer_type>
        struct server_rpc_context : public base_rpc_context<buffer_type, msg_header>
        {
            using base_t = base_rpc_context<buffer_type, msg_header>;
            using buffer_t = buffer_type;
            using success_function_t = std::function<void(char const*, size_t)>;
            using on_error_function_t = std::function<void(net_result const&)>;
            using asio_buffers = std::vector<boost::asio::const_buffer>;
            using post_func_t = std::function<void()>;
            using message_t = std::vector<char>;
            using result_barrier_t = waitable_object;
            using context_t = server_rpc_context<buffer_t>;
            using header_t = msg_header;

            using base_t::base_t;
            // for call
            server_rpc_context(msg_way way, uint64_t hash, buffer_t&& msg)
                : base_t(header_t{ 0,{ false, way == msg_way::one_way }, 0, (uint32_t)(msg.size()), hash },
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
                if(ctx->need_reply() && ctx->have_reply_process())
                    push_call_response(ctx);
                call_list_.push_back(ctx);
                return true;
            }

            void push_call_response(context_ptr& ctx)
            {
                if (ctx->need_reply())
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
                if (ctx->need_reply())
                    push_call_response(ctx);
                call_list_.push_back(ctx);
                return true;
            }

            void push_call_response(context_ptr& ctx)
            {
                if (ctx->need_reply())
                {
                    auto protocol_id = ctx->reply_protocol();
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
                auto itr = call_map_.find(protocol_id);
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