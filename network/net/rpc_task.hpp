#pragma once
#include "async_rpc_channel.hpp"
#include <boost/optional.hpp>

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

        protected:
            rpc_task(client_private_ptr client, context_ptr& ctx)
                : client_(client)
                , ctx_(ctx)
                , dismiss_(false)
            {}

            ~rpc_task()
            {
                do_call_managed();
            }

            rpc_task(rpc_task&& other)
                : client_(other.client_)
                , ctx_(std::move(other.ctx_))
                , dismiss_(other.dismiss_)
            {
                other.dismiss_ = true;
            }

            rpc_task(rpc_task const& other) = default;

            void do_call_managed()
            {
                if (!dismiss_)
                {
                    if (client_->status() == status_t::disconnected)
                    {
                        client_->connect().on_ok([c = client_, ctx = ctx_] {
                                if (c->irouter())
                                {
                                    boost::system::error_code ec;
                                    c->irouter()->connection_incoming(ec, c);
                                }
                                c->call_context_ptr(ctx);
                            })
                            .on_error([c = client_, ctx = ctx_](auto& ec) {
                                if(ctx->on_error)
                                    ctx->on_error(cytx::rpc::exception(error_code::connect_fail, ec.message()));
                                else if(c->irouter())
                                    c->irouter()->connection_incoming(ec, c);
                            });
                    }
                    else
                    {
                        client_->call_context(ctx_);
                    }
                }
            }

            void do_call_and_wait()
            {
                if (!dismiss_)
                {
                    dismiss_ = true;
                    if (client_->status() == status_t::disconnected)
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
                            throw cytx::rpc::exception(error_code::connect_fail, ec.message());
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

        public:
            typed_rpc_task(client_private_ptr client, context_ptr& ctx)
                : base_type(client, ctx)
            {
            }

            typed_rpc_task(typed_rpc_task const&) = default;

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

            void wait(duration_t const& duration = duration_t::max()) &
            {
                if (!this->dismiss_)
                {
                    if (nullptr == result_)
                    {
                        result_ = std::make_shared<result_type>();
                    }

                    this->ctx_->on_ok = [r = result_](char const* data, size_t size)
                    {
                        codec_policy codec{ header_t::big_endian() };
                        *r = codec.template unpack<result_type>(data, size);
                    };
                    this->ctx_->on_error = nullptr;
                }
                this->do_call_and_wait();
            }

            void wait(cytx::rpc::exception& ec, duration_t const& duration = duration_t::max()) &
            {
                try
                {
                    wait(duration);
                }
                catch(cytx::rpc::exception& e)
                {
                    ec = e;
                }
            }

            result_type const& get(duration_t const& duration = duration_t::max()) &
            {
                wait(duration);
                return *result_;
            }

            boost::optional<result_type> get(cytx::rpc::exception& ec, duration_t const& duration = duration_t::max()) &
            {
                boost::optional<result_type> r;
                try
                {
                    wait(duration);
                    r = *result_;
                }
                catch (cytx::rpc::exception& e)
                {
                    ec = e;
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

        public:
            typed_rpc_task(client_private_ptr client, context_ptr& ctx)
                : base_type(client, ctx)
            {
            }

            typed_rpc_task(typed_rpc_task const&) = default;

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

            void wait(duration_t const& duration = duration_t::max()) &
            {
                if (!this->dismiss_)
                {
                    this->ctx_->on_ok = nullptr;
                    this->ctx_->on_error = nullptr;
                }
                this->do_call_and_wait();
            }

            void wait(cytx::rpc::exception& ec, duration_t const& duration = duration_t::max()) &
            {
                try
                {
                    wait(duration);
                }
                catch (cytx::rpc::exception& e)
                {
                    ec = e;
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
            using on_error_func_t = std::function<void(const boost::system::error_code&)>;

        public:
            connect_task(client_private_ptr client)
                : client_(client)
                , dismiss_(false)
            {}

            ~connect_task()
            {
                do_call_managed();
            }

            connect_task(connect_task&& other)
                : client_(other.client_)
                , on_ok_(std::move(other.on_ok_))
                , on_error_(std::move(other.on_error_))
                , barrier_ptr_(std::move(other.barrier_ptr_))
                , result_(other.result_)
                , dismiss_(other.dismiss_)
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

            boost::system::error_code wait(duration_t const& duration = duration_t::max()) &
            {
                if (!this->dismiss_)
                {
                    if (nullptr == result_)
                    {
                        result_ = std::make_shared<boost::system::error_code>();
                    }

                    this->on_ok_ = [this]
                    {
                        barrier_ptr_->notify();
                    };
                    this->on_error_ = [this, r = result_](auto& ec)
                    {
                        *r = ec;
                        barrier_ptr_->notify();
                    };
                }
                this->do_call_and_wait();
                return *result_;
            }

        protected:
            void do_call_managed()
            {
                if (!dismiss_)
                {
                    client_->start(std::move(on_ok_), std::move(on_error_));
                }
            }

            void do_call_and_wait()
            {
                if (!dismiss_)
                {
                    dismiss_ = true;
                    create_barrier();
                    client_->start(std::move(on_ok_), std::move(on_error_));
                    barrier_wait();
                }
            }

            void create_barrier()
            {
                if (!barrier_ptr_)
                    barrier_ptr_ = std::make_unique<ios_result_barrier>(client_->get_io_service());
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
            std::unique_ptr<ios_result_barrier> barrier_ptr_;
            std::shared_ptr<boost::system::error_code> result_;
        };
    }
}