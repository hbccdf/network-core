#pragma once
#include "async_connection.hpp"
#include "wait_barrier.hpp"
#include "async_rpc_context.hpp"
#include "router.hpp"
#include "irouter.hpp"

namespace cytx {
    namespace rpc
    {
        using ios_t = ios_wrapper;

        template <typename context_type>
        inline auto make_rpc_context(ios_t& ios, uint64_t name, bool inter_proto, typename context_type::buffer_t&& buffer)
            -> std::enable_if_t<!std::is_same<typename context_type::header_t, client_msg_header>::value, std::shared_ptr<context_type>>
        {
            using context_t = context_type;
            if(!inter_proto)
                return std::make_shared<context_t>(ios, msg_way::two_way, name, std::move(buffer));
            else
                return std::make_shared<context_t>(ios, (uint32_t)name, std::move(buffer));
        }

        template <typename context_type>
        inline auto make_rpc_context(ios_t& ios, uint64_t name, bool inter_proto, typename context_type::buffer_t&& buffer)
            -> std::enable_if_t<std::is_same<typename context_type::header_t, client_msg_header>::value, std::shared_ptr<context_type>>
        {
            using context_t = context_type;
            return std::make_shared<context_t>(ios, (uint32_t)name, std::move(buffer));
        }

        template <typename header_type, typename CodecPolicy, typename Protocol, typename ... Args>
        inline auto make_rpc_context(ios_t& ios, CodecPolicy const& cp, Protocol const& protocol, Args&& ... args)
        {
            using context_t = get_context_t<typename CodecPolicy::buffer_type, header_type>;
            auto buffer = protocol.pack_args(cp, std::forward<Args>(args)...);
            return make_rpc_context<context_t>(ios, protocol.proto(), protocol.inter_protocol(), std::move(buffer));
        }

        template <typename header_type, typename CodecPolicy, typename ... Args>
        inline auto make_rpc_context(ios_t& ios, CodecPolicy const& cp, uint64_t proto, bool is_inter_protocol, Args&& ... args)
        {
            using context_t = get_context_t<typename CodecPolicy::buffer_type, header_type>;
            auto buffer = cp.pack_args(std::forward<Args>(args)...);
            return make_rpc_context<context_t>(ios, proto, is_inter_protocol, std::move(buffer));
        }

        template<typename CodecPolicy, typename ConnectPolicy, typename header_type, typename Ret>
        class typed_rpc_task;

        template<typename CodePolicy, typename header_type>
        class connect_task;

        enum class status_t
         {
             disconnected,
             connecting,
             connected,
             disconnecting,
         };

        template <typename CodecPolicy, typename header_type>
        class async_rpc_channel;

        template<typename CodecPolicy, typename header_type>
        class rpc_call
        {
            using connect_t = async_rpc_channel<CodecPolicy, header_type>;
            using connect_ptr = connect_t*;
            using codec_policy = CodecPolicy;
        protected:
            rpc_call(connect_ptr conn) : conn_(conn)
            {
            }

            void set_conn_ptr(connect_ptr conn)
            {
                conn_ = conn;
            }
        public:
            template <typename Protocol, typename ... Args>
            auto call(Protocol const& protocol, Args&& ... args)
            {
                return conn_->call_impl<codec_policy>(protocol, std::forward<Args>(args)...);
            }

            template <typename CallCodecPolicy, typename Protocol, typename ... Args>
            auto call(Protocol const& protocol, Args&& ... args)
            {
                return conn_->call_impl<CallCodecPolicy>(protocol, std::forward<Args>(args)...);
            }

            template <typename result_type, typename ... Args>
            auto free_call(std::string name, Args&& ... args)
            {
                return conn_->free_call_impl<codec_policy, result_type>(std::hash<string>{}(name), false, std::forward<Args>(args)...);
            }

            template <typename result_type, typename ... Args>
            auto free_call(uint32_t proto, Args&& ... args)
            {
                return conn_->free_call_impl<codec_policy, result_type>(proto, true, std::forward<Args>(args)...);
            }

            template <typename CallCodecPolicy, typename result_type, typename ... Args>
            auto free_call(std::string name, Args&& ... args)
            {
                return conn_->free_call_impl<CallCodecPolicy, result_type>(std::hash<string>{}(name), false, std::forward<Args>(args)...);
            }

            template <typename CallCodecPolicy, typename result_type, typename ... Args>
            auto free_call(uint32_t proto, Args&& ... args)
            {
                return conn_->free_call_impl<CallCodecPolicy, result_type>((uint64_t)proto, true, std::forward<Args>(args)...);
            }
        private:
            connect_ptr conn_;
        };

        template <typename CodecPolicy, typename header_type>
        class async_rpc_channel : public rpc_call<CodecPolicy, header_type>,
            public std::enable_shared_from_this<async_rpc_channel<CodecPolicy, header_type>>
        {
        public:
            using base_t = rpc_call<CodecPolicy, header_type>;
            using ios_t = ios_wrapper;
            using codec_policy = CodecPolicy;
            using header_t = header_type;
            using connect_t = async_rpc_channel<codec_policy, header_t>;
            using rpc_call_container_t = base_call_container<codec_policy, header_type>;
            using context_t = typename rpc_call_container_t::context_t;
            using context_ptr = typename rpc_call_container_t::context_ptr;
            using call_list_t = typename rpc_call_container_t::call_list_t;
            using call_map_t = typename rpc_call_container_t::call_map_t;
            using router_t = typename router<codec_policy, header_t>;
            using irouter_t = irouter<connect_t>;
            using irouter_ptr = irouter_t*;

            template <typename Ret>
            using rpc_task_alias = typed_rpc_task<codec_policy, codec_policy, header_t, Ret>;

        public:
            async_rpc_channel(ios_t& ios, tcp::endpoint const& endpoint, router_t& router, irouter_ptr irptr = nullptr)
                : base_t(this)
                , ios_(ios)
                , connection_(ios.service(), endpoint)
                , status_(status_t::disconnected)
                , read_buffer_(1024)
                , router_(router)
                , is_client_(true)
                , irptr_(irptr)
            {
            }

            async_rpc_channel(ios_t& ios, router_t& router, irouter_ptr irptr = nullptr)
                : base_t(this)
                , ios_(ios)
                , connection_(ios.service())
                , status_(status_t::disconnected)
                , read_buffer_(1024)
                , router_(router)
                , is_client_(false)
                , irptr_(irptr)
            {
            }

            ~async_rpc_channel()
            {
                if (status_ != status_t::disconnected)
                {
                    on_error(error_code::cancel);
                }
            }

            void async_rpc_channel::set_no_delay()
            {
                boost::asio::ip::tcp::no_delay option(true);
                boost::system::error_code ec;
                connection_.socket().set_option(option, ec);
            }

            tcp::socket& socket()
            {
                return connection_.socket();
            }

            status_t status() const
            {
                return status_;
            }

            auto connect()
            {
                using rpc_task_t = connect_task<codec_policy, header_t>;
                return rpc_task_t{ this->shared_from_this() };
            }

            auto disconnect()
            {
                on_error(error_code::be_disconnected);
            }

            void start(std::function<void()>&& on_success,
                std::function<void(const boost::system::error_code&)>&& on_error_func)
            {
                lock_t lock{ connect_mutex_ };
                if (status_ != status_t::disconnected)
                {
                    lock.unlock();
                    boost::system::error_code ec(1, boost::system::generic_category());
                    if (on_error_func)
                        on_error_func(ec);
                    else if (irptr_)
                    {
                        irptr_->connection_incoming(ec, this->shared_from_this());
                    }
                    return;
                }

                status_ = status_t::connecting;

                using on_error_func_t = std::function<void(const boost::system::error_code&)>;
                using on_success_func_t = std::function<void()>;
                connection_.start(
                    [this, on_succ = std::move(on_success)] // when successfully connected
                {
                    start_rpc_service();
                    if (on_succ)
                        on_succ();
                    else if (irptr_)
                    {
                        boost::system::error_code ec;
                        irptr_->connection_incoming(ec, this->shared_from_this());
                    }
                },
                    [this, on_err = std::move(on_error_func)](const boost::system::error_code& ec) // when failed to connect
                {
                    stop_rpc_service(error_code::connect_fail);
                    if(on_err)
                        on_err(ec);
                    else if (irptr_)
                    {
                        irptr_->connection_incoming(ec, this->shared_from_this());
                    }
                });
            }

            void start()
            {
                lock_t lock{ connect_mutex_ };
                if (status_ != status_t::disconnected)
                {
                    return;
                }

                start_rpc_service();
            }

            void close(bool normal_close = true)
            {
                if (normal_close)
                {
                    boost::system::error_code ec;
                    connection_.socket().shutdown(boost::asio::socket_base::shutdown_both, ec);
                }
                boost::system::error_code ignored_ec;
                connection_.socket().close(ignored_ec);
            }

            void on_error(error_code err)
            {
                boost::system::error_code ec(1, boost::system::generic_category());
                on_error(ec, err);
            }

            void on_error(const boost::system::error_code& ec, error_code err = error_code::badconnect)
            {
                stop_rpc_service(err);
                if(!is_client_)
                    router_.on_error(this->shared_from_this(), ec);
                if (irptr_)
                    irptr_->connection_terminated(ec, this->shared_from_this(), err);
            }

            template <typename CallCodecPolicy, typename Protocol, typename ... Args>
            auto call_impl(Protocol const& protocol, Args&& ... args)
            {
                static_assert(is_rpc_protocol<Protocol>::value, "Illegal protocol for rpc call!");

                using result_type = typename Protocol::result_type;
                using rpc_task_t = typed_rpc_task<CallCodecPolicy, codec_policy, header_t, result_type>;
                CallCodecPolicy cp{ header_t::big_endian() };
                auto ctx = rpc::make_rpc_context<header_t>(ios_, cp, protocol, std::forward<Args>(args)...);
                return rpc_task_t{ this->shared_from_this(), ctx };
            }

            template <typename CallCodecPolicy, typename result_type, typename ... Args>
            auto free_call_impl(uint64_t proto, bool is_inter_proto, Args&& ... args)
            {
                using rpc_task_t = typed_rpc_task<CallCodecPolicy, codec_policy, header_t, result_type>;
                CallCodecPolicy cp{ header_t::big_endian() };
                auto ctx = rpc::make_rpc_context<header_t>(ios_, cp, proto, is_inter_proto, std::forward<Args>(args)...);
                return rpc_task_t{ this->shared_from_this(), ctx };
            }

            void call_context(context_ptr& ctx)
            {
                lock_t lock_connect{ connect_mutex_ };
                if (status_ != status_t::connected && status_ != status_t::connecting)
                {
                    ctx->error(error_code::badconnect, "rpc session already stopped");
                    return;
                }

                lock_t lock{ mutex_ };
                bool empty = calls_.call_list_empty();
                auto push_success = calls_.push_call(ctx);
                lock.unlock();

                if (!push_success)
                {
                    ctx->error(error_code::unknown);
                }

                if (empty && status_t::connected == status_)
                {
                    get_io_service().service().post([this]
                    {
                        if (to_calls_.empty())
                        {
                            call_impl();
                        }
                    });
                }
            }

            void call_context_ptr(context_ptr ctx)
            {
                call_context(ctx);
            }

            void response(context_ptr& ctx)
            {
                call_context(ctx);
            }

            void send(header_t& header, const char* const data)
            {
                using buffer_t = typename context_t::buffer_t;
                auto ctx = std::make_shared<context_t>(ios_, header, buffer_t{ data, data + header.length() });
                call_context(ctx);
            }

            void cancel(context_ptr const& ctx)
            {
                get_io_service().post([this, ctx]
                {
                    lock_t lock{ mutex_ };
                    calls_.get_call_from_map(ctx->head.call_id());
                    lock.unlock();
                    ctx->error(error_code::cancel, "rpc canceled by client");
                });
            }

            void start_rpc_service()
            {
                status_ = status_t::connected;
                set_no_delay();
                call_impl();
                recv_head();
            }

            void stop_rpc_service(error_code error)
            {
                lock_t lock{ connect_mutex_ };
                status_ = status_t::disconnecting;
                close(error != error_code::badconnect && error != error_code::connect_fail);
                stop_rpc_calls(error);
                status_ = status_t::disconnected;
            }

            void call_impl()
            {
                lock_t lock{ mutex_ };
                if (!calls_.call_list_empty())
                {
                    calls_.task_calls_from_list(to_calls_);
                    call_impl1();
                }
            }

            void call_impl1()
            {
                auto& to_call = to_calls_.front();

                to_call->hton();
                async_write(connection_.socket(), to_call->get_send_message(),
                    boost::bind(&async_rpc_channel::handle_send, this->shared_from_this(), asio_error, to_call));
            }

            void recv_head()
            {
                async_read(connection_.socket(), boost::asio::buffer(&head_, sizeof(head_)),
                    boost::bind(&async_rpc_channel::handle_recv_head, this->shared_from_this(), asio_error));
            }

            void recv_body()
            {
                if (head_.is_reply())
                {
                    recv_call_reply();
                }
                else
                {
                    recv_request();
                }
            }

            void recv_call_reply()
            {
                auto call_id = head_.call_id();
                lock_t lock{ mutex_ };
                auto call_ctx = calls_.get_call_from_map(call_id);
                lock.unlock();

                if (call_ctx == nullptr || (!call_ctx->on_ok && !call_ctx->on_error && !call_ctx->barrier_ptr))
                {
                    recv_request();
                    return;
                }

                if (0 == head_.length())
                {
                    call_complete(call_ctx);
                }
                else if (head_.length() > 10240)
                {
                    on_error(error_code::invalid_header);
                }
                else
                    async_read(connection_.socket(), call_ctx->get_recv_message(head_.length()), boost::bind(&async_rpc_channel::handle_recv_body,
                        this->shared_from_this(), call_ctx, asio_error));
            }

            void recv_request()
            {
                if (0 == head_.length())
                {
                    router_.on_read(this->shared_from_this());
                    recv_head();
                }
                else if (head_.length() > 10240)
                {
                    on_error(error_code::invalid_header);
                }
                else
                {
                    if (head_.length() > 1024)
                        read_buffer_.resize(head_.length());
                    async_read(connection_.socket(), boost::asio::buffer(read_buffer_.data(), head_.length()), boost::bind(&async_rpc_channel::handle_recv_body,
                        this->shared_from_this(), asio_error));
                }
            }

            void call_complete(context_ptr& ctx)
            {
                if (nullptr != ctx)
                {
                    auto rcode = ctx->head.result();
                    if (rcode < (uint16_t)result_code::fail)
                    {
                        ctx->ok();
                    }
                    else
                    {
                        codec_policy cp{ header_t::big_endian() };
                        auto recv_error = cp.template unpack<exception>(ctx->recv_msg.data(), ctx->recv_msg.size());
                        ctx->error(std::move(recv_error));
                    }
                }

                recv_head();
            }

            void stop_rpc_calls(error_code error)
            {
                to_calls_.clear();
                call_map_t to_responses;
                lock_t lock{ mutex_ };
                calls_.task_calls_from_map(to_responses);
                lock.unlock();
                stop_rcp_calls_impl<call_map_t>(to_responses, error);
            }

            template<typename call_map_type>
            auto stop_rcp_calls_impl(call_map_type& to_responses, error_code error)
                -> std::enable_if_t<std::is_same<header_t, msg_header>::value, void>
            {
                for (auto& elem : to_responses)
                {
                    auto ctx = elem.second;
                    ctx->error(error);
                }
            }

            template<typename call_map_type>
            auto stop_rcp_calls_impl(call_map_type& to_responses, error_code error)
                -> std::enable_if_t<!std::is_same<header_t, msg_header>::value, void>
            {
                for (auto& elem : to_responses)
                {
                    auto& que = elem.second;
                    while (!que.empty())
                    {
                        auto ctx = que.front();
                        que.pop();
                        ctx->error(error);
                    }
                }
            }

            void handle_send(boost::system::error_code const& error, context_ptr ctx)
            {
                if (!connection_.socket().is_open())
                    return;

                to_calls_.pop_front();

                ctx->ntoh();
                if (error)
                {
                    on_error(error);
                }
                else
                {
                    if (ctx->is_reply())
                        ctx->apply_post_func();
                    ctx.reset();

                    if (to_calls_.empty())
                    {
                        call_impl();
                    }
                    else
                    {
                        call_impl1();
                    }
                }
            }

            void handle_recv_head(boost::system::error_code const& error)
            {
                if (!connection_.socket().is_open())
                    return;

                head_.ntoh();
                if (!error)
                {
                    recv_body();
                }
                else
                {
                    on_error(error);
                }
            }

            void handle_recv_body(context_ptr ctx, boost::system::error_code const& error)
            {
                if (!connection_.socket().is_open())
                    return;

                if (!error)
                {
                    call_complete(ctx);
                }
                else
                {
                    on_error(error);
                }
            }

            void handle_recv_body(boost::system::error_code const& error)
            {
                if (!connection_.socket().is_open())
                    return;

                if (!error)
                {
                    router_.on_read(this->shared_from_this());
                    recv_head();
                }
                else
                {
                    on_error(error);
                }
            }

            header_t const& get_read_header() const
            {
                return head_;
            }

            header_t& get_read_header()
            {
                return head_;
            }

            blob_t get_read_buffer() const
            {
                return { read_buffer_.data(), (size_t)head_.length() };
            }

            ios_t& get_io_service()
            {
                return ios_;
            }

            codec_policy get_codec_policy() const
            {
                return{};
            }

            irouter_ptr irouter() const
            {
                return irptr_;
            }

        private:
            ios_t& ios_;
            async_connection connection_;
            rpc_call_container_t calls_;
            status_t status_;
            header_t head_{};
            std::vector<char> read_buffer_;
            call_list_t to_calls_;
            mutable std::mutex mutex_;
            mutable std::mutex connect_mutex_;
            router_t& router_;
            bool is_client_;
            irouter_ptr irptr_;
        };
    }
}