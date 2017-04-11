#pragma once
#include "async_rpc_channel.hpp"
#include <boost/optional.hpp>
#include "schedule_timer.hpp"

namespace cytx
{
    namespace rpc
    {
        template <typename CodecPolicy, typename header_type>
        class rpc_task
        {
        public:
            using codec_policy = CodecPolicy;
            using client_private_t = async_rpc_channel<codec_policy, header_type>;
            using client_private_ptr = std::shared_ptr<client_private_t>;
            using context_ptr = typename client_private_t::context_ptr;
            using header_t = header_type;
            using timer_t = schedule_timer;
            using duration_t = typename timer_t::duration_t;
            using timer_ptr = std::shared_ptr<timer_t>;

        protected:
            rpc_task(client_private_ptr client, context_ptr& ctx)
                : client_(client)
                , ctx_(ctx)
                , dismiss_(false)
                , duration_(0)
            {}

            ~rpc_task()
            {
                do_call_managed();
            }

            rpc_task(rpc_task&& other)
                : client_(std::move(other.client_))
                , ctx_(std::move(other.ctx_))
                , dismiss_(other.dismiss_)
                , timer_ptr_(std::move(other.timer_ptr_))
                , duration_(std::move(other.duration_))
            {
                other.dismiss_ = true;
            }

            rpc_task(rpc_task const& other) = default;

            void do_call_managed()
            {
                if (!dismiss_)
                {
                    dismiss_ = true;
                    if (client_->is_client() && client_->status() == status_t::disconnected)
                    {
                        auto on_ok_func = [c = client_, ctx = ctx_]() mutable {
                            if (c->irouter())
                            {
                                c->irouter()->connection_incoming(rpc_result(), c);
                            }
                            c->call_context(ctx);
                        };

                        auto on_error_func = [c = client_, ctx = ctx_](auto& ec) {
                            if (ctx->on_ok || ctx->on_error || ctx->barrier_ptr)
                                ctx->error(ec);
                            else if (c->irouter())
                                c->irouter()->connection_incoming(ec, c);
                        };

                        client_->connect().on_ok(on_ok_func).on_error(on_error_func).delay(duration_);
                    }
                    else if(!timer_ptr_ || duration_.count() <= 0)
                    {
                        client_->call_context(ctx_);
                    }
                    else
                    {
                        auto on_timer_func = [c = std::move(client_), ctx = std::move(ctx_), t = timer_ptr_](const boost::system::error_code& ec) mutable {
                            t.reset();
                            if (!ec)
                            {
                                c->call_context(ctx);
                            }
                        };
                        timer_ptr_->async_wait(duration_, on_timer_func);
                    }
                }
            }

            void do_call_and_wait()
            {
                if (!dismiss_)
                {
                    dismiss_ = true;
                    if (client_->is_client() && client_->status() == status_t::disconnected)
                    {
                        auto task = client_->connect();
                        auto ec = task.wait();
                        if (!ec)
                        {
                            ctx_->create_barrier();
                            client_->call_context(ctx_);
                            ctx_->wait();
                        }
                        else
                        {
                            throw cytx::rpc::rpc_exception(ec);
                        }
                    }
                    else
                    {
                        ctx_->create_barrier();
                        client_->call_context(ctx_);
                        ctx_->wait();
                    }
                }
            }

        public:
            void cancel()
            {
                this->client_->cancel(ctx_);
            }

            header_t& header()
            {
                return this->ctx_->get_head();
            }

            const header_t& header() const
            {
                return this->ctx_->head;
            }

        protected:
            client_private_ptr client_;
            context_ptr ctx_;
            bool dismiss_;
            timer_ptr timer_ptr_;
            duration_t duration_;
        };

        template <typename CodecPolicy, typename ConnectPolicy, typename header_type, typename Ret>
        class typed_rpc_task : public rpc_task<ConnectPolicy, header_type>
        {
        public:
            using codec_policy = CodecPolicy;
            using base_type = rpc_task<ConnectPolicy, header_type>;
            using result_type = Ret;
            using client_private_t = typename base_type::client_private_t;
            using client_private_ptr = typename base_type::client_private_ptr;
            using context_ptr = typename base_type::context_ptr;
            using header_t = header_type;
            using timer_t = typename base_type::timer_t;
            using duration_t = typename base_type::duration_t;

        public:
            typed_rpc_task(client_private_ptr client, context_ptr& ctx)
                : base_type(client, ctx)
            {
            }

            typed_rpc_task(typed_rpc_task const&) = default;

            typed_rpc_task&& reply(uint32_t reply_id) &&
            {
                ctx_->reply_protocol_id = reply_id;
                return std::move(*this);
            }

            template<typename T>
            auto reply(T reply_id) && -> std::enable_if_t<std::is_enum<T>::value, typed_rpc_task&&>
            {
                ctx_->reply_protocol_id = (uint32_t)reply_id;
                return std::move(*this);
            }

            typed_rpc_task& reply(uint32_t reply_id) &
            {
                ctx_->reply_protocol_id = reply_id;
                return *this;
            }

            template<typename T>
            auto reply(T reply_id) & ->std::enable_if_t<std::is_enum<T>::value, typed_rpc_task&>
            {
                ctx_->reply_protocol_id = (uint32_t)reply_id;
                return *this;
            }

            template <typename F>
            typed_rpc_task&& on_ok(F&& f) &&
            {
                if (nullptr == result_)
                {
                    result_ = std::make_shared<result_type>();
                }

                this->ctx_->on_ok = [func = std::forward<F>(f), r = result_](char const* data, size_t size)
                {
                    codec_policy codec{ header_t::big_endian() };
                    *r = codec.template unpack<result_type>(data, size);
                    func(*r);
                };

                return std::move(*this);
            }

            template <typename F>
            typed_rpc_task&& on_error(F&& f) &&
            {
                this->ctx_->on_error = std::forward<F>(f);
                return std::move(*this);
            }

            typed_rpc_task&& delay(const duration_t& duration) &&
            {
                if (duration.count() <= 0)
                    return std::move(*this);

                if (!this->timer_ptr_)
                    timer_ptr_ = std::make_shared<timer_t>(client_->get_io_service());
                duration_ = duration;
                return std::move(*this);
            }

            typed_rpc_task&& delay(int milliseconds) &&
            {
                return std::move(*this).delay(duration_t(milliseconds));
            }

            typed_rpc_task&& timeout(const duration_t& duration) &&
            {
                this->ctx_->timeout(duration);
                return std::move(*this);
            }

            typed_rpc_task&& timeout(int milliseconds) &&
            {
                return std::move(*this).timeout(duration_t(milliseconds));
            }

            void wait(duration_t const& duration = duration_t::max()) &
            {
                if (!this->dismiss_)
                {
                    if (nullptr == result_)
                    {
                        result_ = std::make_shared<result_type>();
                    }
                    this->ctx_->timeout(duration);
                    this->ctx_->on_ok = [r = result_](char const* data, size_t size)
                    {
                        codec_policy codec{ header_t::big_endian() };
                        *r = codec.template unpack<result_type>(data, size);
                    };
                    this->ctx_->on_error = nullptr;
                }
                this->do_call_and_wait();
            }

            void wait(cytx::rpc::rpc_result& ec, duration_t const& duration = duration_t::max()) &
            {
                try
                {
                    wait(duration);
                }
                catch(cytx::rpc::rpc_exception& e)
                {
                    ec = e.result();
                }
            }

            result_type const& get(duration_t const& duration = duration_t::max()) &
            {
                wait(duration);
                return *result_;
            }

            boost::optional<result_type> get(cytx::rpc::rpc_result& ec, duration_t const& duration = duration_t::max()) &
            {
                boost::optional<result_type> r;
                try
                {
                    wait(duration);
                    r = *result_;
                }
                catch (cytx::rpc::rpc_exception& e)
                {
                    ec = e.result();
                }
                return r;
            }

        private:
            std::shared_ptr<result_type> result_;
        };

        template <typename CodecPolicy, typename ConnectPolicy, typename header_type>
        class typed_rpc_task<CodecPolicy, ConnectPolicy, header_type, void> : public rpc_task<ConnectPolicy, header_type>
        {
        public:
            using codec_policy = CodecPolicy;
            using base_type = rpc_task<ConnectPolicy, header_type>;
            using result_type = void;
            using client_private_t = typename base_type::client_private_t;
            using client_private_ptr = typename base_type::client_private_ptr;
            using context_ptr = typename base_type::context_ptr;
            using header_t = typename base_type::header_t;
            using timer_t = typename base_type::timer_t;
            using duration_t = typename base_type::duration_t;

        public:
            typed_rpc_task(client_private_ptr client, context_ptr& ctx)
                : base_type(client, ctx)
            {
            }

            typed_rpc_task(typed_rpc_task const&) = default;

            typed_rpc_task&& reply(uint32_t reply_id) &&
            {
                ctx_->reply_protocol_id = reply_id;
                return std::move(*this);
            }

            template<typename T>
            auto reply(T reply_id) && ->std::enable_if_t<std::is_enum<T>::value, typed_rpc_task&&>
            {
                ctx_->reply_protocol_id = (uint32_t)reply_id;
                return std::move(*this);
            }

            typed_rpc_task& reply(uint32_t reply_id) &
            {
                ctx_->reply_protocol_id = reply_id;
                return *this;
            }

            template<typename T>
            auto reply(T reply_id) & ->std::enable_if_t<std::is_enum<T>::value, typed_rpc_task&>
            {
                ctx_->reply_protocol_id = (uint32_t)reply_id;
                return *this;
            }

            template <typename F>
            typed_rpc_task&& on_ok(F&& f) &&
            {
                this->ctx_->on_ok = [func = std::forward<F>(f)](char const* data, size_t size) { func(); };
                return std::move(*this);
            }

            template <typename F>
            typed_rpc_task&& on_error(F&& f) &&
            {
                this->ctx_->on_error = std::forward<F>(f);
                return std::move(*this);
            }

            typed_rpc_task&& delay(const duration_t& duration) &&
            {
                if (duration.count() <= 0)
                    return std::move(*this);

                if (!this->timer_ptr_)
                    timer_ptr_ = std::make_shared<timer_t>(client_->get_io_service());
                duration_ = duration;
                return std::move(*this);
            }

            typed_rpc_task&& delay(int milliseconds) &&
            {
                return std::move(*this).delay(duration_t(milliseconds));
            }

            typed_rpc_task&& timeout(const duration_t& duration) &&
            {
                this->ctx_->timeout(duration);
                return std::move(*this);
            }

            typed_rpc_task&& timeout(int milliseconds) &&
            {
                return std::move(*this).timeout(duration_t(milliseconds));
            }

            void wait(duration_t const& duration = duration_t::max()) &
            {
                if (!this->dismiss_)
                {
                    this->ctx_->timeout(duration);
                    this->ctx_->on_ok = nullptr;
                    this->ctx_->on_error = nullptr;
                }
                this->do_call_and_wait();
            }

            void wait(cytx::rpc::rpc_result& ec, duration_t const& duration = duration_t::max()) &
            {
                try
                {
                    wait(duration);
                }
                catch (cytx::rpc::rpc_exception& e)
                {
                    ec = e.result();
                }
            }
        };

        template <typename CodecPolicy, typename header_type>
        class connect_task
        {
        public:
            using codec_policy = CodecPolicy;
            using client_private_t = async_rpc_channel<codec_policy, header_type>;
            using client_private_ptr = std::shared_ptr<client_private_t>;
            using on_ok_func_t = std::function<void()>;
            using on_error_func_t = std::function<void(const rpc_result&)>;
            using timer_t = schedule_timer;
            using duration_t = typename timer_t::duration_t;
            using timer_ptr = std::shared_ptr<timer_t>;

        public:
            connect_task(client_private_ptr client)
                : client_(client)
                , dismiss_(false)
                , timer_ptr_(std::make_shared<timer_t>(client_->get_io_service()))
                , timer_timeout_ptr_(std::make_shared<timer_t>(client_->get_io_service()))
                , timeout_duration_(0)
                , duration_(0)
            {}

            ~connect_task()
            {
                do_call_managed();
            }

            connect_task(connect_task&& other)
                : client_(std::move(other.client_))
                , on_ok_(std::move(other.on_ok_))
                , on_error_(std::move(other.on_error_))
                , barrier_ptr_(std::move(other.barrier_ptr_))
                , result_(std::move(other.result_))
                , dismiss_(other.dismiss_)
                , timer_ptr_(std::move(other.timer_ptr_))
                , duration_(std::move(other.duration_))
            {
                other.dismiss_ = true;
            }

            connect_task(connect_task const& other) = default;

            connect_task&& on_ok(on_ok_func_t&& f) &&
            {
                this->on_ok_ = std::forward<on_ok_func_t>(f);
                return std::move(*this);
            }

            connect_task&& on_error(on_error_func_t&& f) &&
            {
                this->on_error_ = std::forward<on_error_func_t>(f);
                return std::move(*this);
            }

            connect_task&& delay(const duration_t& duration) &&
            {
                if (duration.count() > 0)
                duration_ = duration;
                return std::move(*this);
            }

            connect_task&& delay(int milliseconds) &&
            {
                return std::move(*this).delay(duration_t(milliseconds));
            }

            connect_task&& timeout(const duration_t& duration) &&
            {
                if (duration.count() > 0)
                    timeout_duration_ = duration;
                return std::move(*this);
            }

            connect_task&& timeout(int milliseconds) &&
            {
                return std::move(*this).delay(duration_t(milliseconds));
            }

            rpc_result wait(duration_t const& duration = duration_t::max()) &
            {
                if (!this->dismiss_)
                {
                    if (duration != duration_t::max())
                    {
                        timeout_duration_ = duration;
                    }

                    if (nullptr == result_)
                    {
                        result_ = std::make_shared<rpc_result>();
                    }

                    create_barrier();

                    this->on_ok_ = [b = barrier_ptr_]
                    {
                        if (b->is_over())
                            return;
                        b->notify();
                    };
                    this->on_error_ = [b = barrier_ptr_, r = result_](auto& ec)
                    {
                        if (b->is_over())
                            return;
                        *r = ec;
                        b->notify();
                    };
                }
                this->do_call_and_wait();
                return *result_;
            }

        protected:
            void do_call_managed()
            {
                if (dismiss_)
                    return;
                dismiss_ = true;
                if (!timer_ptr_ || duration_.count() <= 0)
                {
                    client_->start(std::move(on_ok_), std::move(on_error_));
                }
                else
                {
                    auto on_timer_func = [c = std::move(client_), on_ok_f = std::move(on_ok_), on_err_f = std::move(on_error_), t = timer_ptr_](const boost::system::error_code& ec) mutable {
                        t.reset();
                        if (!ec)
                        {
                            c->start(std::move(on_ok_f), std::move(on_err_f));
                        }
                    };
                    timer_ptr_->async_wait(duration_, on_timer_func);
                }
            }

            void do_call_and_wait()
            {
                if (!dismiss_)
                {
                    dismiss_ = true;
                    if (timeout_duration_.count() > 0)
                    {
                        auto on_timer_func = [b = barrier_ptr_, r = result_](auto& ec) {
                            if (b->is_over())
                                return;
                            if (ec)
                            {
                                *r = ec;
                            }
                            else
                            {
                                *r = rpc_result(error_code::timeout);
                            }
                            b->notify();
                        };
                        timer_timeout_ptr_->async_wait(timeout_duration_, on_timer_func);
                    }
                    client_->start(std::move(on_ok_), std::move(on_error_));
                    barrier_wait();
                }
            }

            void create_barrier()
            {
                if (!barrier_ptr_)
                    barrier_ptr_ = std::make_shared<ios_result_barrier>(client_->get_io_service());
            }

            void barrier_wait()
            {
                barrier_ptr_->wait();
            }

        protected:
            client_private_ptr client_;
            bool dismiss_;
            on_ok_func_t on_ok_;
            on_error_func_t on_error_;
            std::shared_ptr<ios_result_barrier> barrier_ptr_;
            std::shared_ptr<rpc_result> result_;
            timer_ptr timer_ptr_;
            duration_t duration_;
            timer_ptr timer_timeout_ptr_;
            duration_t timeout_duration_;
        };
    }
}