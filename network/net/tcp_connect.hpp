#pragma once
#include <memory>
#include <queue>
#include <string>
#include <atomic>
#include <boost/asio.hpp>
#include <fmt/format.h>
#include "network/base/log.hpp"
#include "network/base/world.hpp"
#include "network/util/net.hpp"
#include "network/timer/timer_manager.hpp"
#include "raw_msg.hpp"
#include "irouter_base.hpp"
#include "async_func.hpp"

#define CLIENT_MSG_BUFF_MAX 1024 * 100 //100K

HAS_FIELD(call_id);

namespace cytx
{
    namespace net
    {
        namespace detail
        {
            template<typename MSG>
            class connection_reader
            {
                using this_t = connection_reader<MSG>;
            public:
                using msg_t = MSG;
                using header_t = typename msg_t::header_t;
                using msg_ptr = std::shared_ptr<msg_t>;
                using buffers_t = boost::asio::mutable_buffers_1;

                connection_reader(cytx::log_ptr_t log_ptr, int32_t conn_id)
                    : log_(log_ptr)
                    , conn_id_(conn_id)
                {
                }
            public:
                buffers_t prepare_next_recv()
                {
                    cur_index_ = 0;
                    msg_list_.clear();
                    return boost::asio::buffer(boost::asio::buffer(buff_) + remain_len_);
                }

                size_t completion_condition(const boost::system::error_code& err, size_t bytes_transfered)
                {
                    if (err)
                        return 0;

                    size_t data_len = remain_len_ + bytes_transfered;
                    log_->trace("connection {} completion condition, bytes {}, total len {}", conn_id_, bytes_transfered, data_len);

                    assert(data_len <= CLIENT_MSG_BUFF_MAX);

                    if ((size_t)-1 == cur_len_ && data_len >= header_length_)
                    {
                        if (!parse_header(buff_.begin()))
                            return 0;
                    }

                    return data_len >= cur_len_ ? 0 : boost::asio::detail::default_max_transfer_size;
                }

                bool parse(size_t bytes_transfered)
                {
                    remain_len_ += bytes_transfered;
                    auto pnext = buff_.begin();
                    assert(remain_len_ <= CLIENT_MSG_BUFF_MAX);

                    size_t total_len = remain_len_;
                    size_t msg_count = 0;

                    bool parse_ok = true;
                    while (parse_ok)
                    {
                        if ((size_t)-1 != cur_len_)
                        {
                            if (cur_len_ > CLIENT_MSG_BUFF_MAX || cur_len_ < header_length_)
                            {
                                parse_ok = false;
                            }
                            else if (remain_len_ >= cur_len_)
                            {
                                msg_ptr msg = std::make_shared<msg_t>(header_);
                                if (header_.length > 0)
                                {
                                    memcpy(msg->raw_data(), std::next(pnext, header_length_), header_.length);
                                }
                                msg_list_.push_back(msg);
                                ++msg_count;

                                std::advance(pnext, cur_len_);
                                remain_len_ -= cur_len_;
                                cur_index_ += cur_len_;
                                cur_len_ = -1;
                            }
                            else
                            {
                                break;
                            }
                        }
                        else if (remain_len_ >= header_length_)
                        {
                            parse_header(pnext);
                        }
                        else
                        {
                            break;
                        }
                    }

                    if (pnext == buff_.begin())
                    {
                        //we should have at least got one msg.
                        parse_ok = false;
                    }
                    else if (parse_ok && remain_len_ > 0)
                    {
                        //left behind unparsed msg
                        memcpy(buff_.begin(), pnext, remain_len_);
                    }

                    log_->trace("connection {}, parse msg, bytes {}, total {}, msg count {}, remain_len {}", conn_id_, bytes_transfered, total_len, msg_count, remain_len_);

                    return parse_ok;
                }

                void reset()
                {
                    cur_len_ = -1;
                    remain_len_ = 0;
                    cur_index_ = 0;
                    header_.length = 0;
                    msg_list_.clear();
                    is_valid_ = true;
                }

                const std::vector<msg_ptr>& get_msgs() const { return msg_list_; }

                bool is_valid() const { return is_valid_ && cur_len_ <= CLIENT_MSG_BUFF_MAX; }

            private:
                bool parse_header(const void* src)
                {
                    memcpy(&header_, src, header_length_);
                    header_.ntoh();
                    cur_len_ = header_length_ + header_.length;

                    if (cur_len_ > CLIENT_MSG_BUFF_MAX || cur_len_ < header_length_)
                    {
                        is_valid_ = false;
                        log_->error("connection {}, completion_condition: {}", conn_id_, cur_len_);
                        return false;
                    }
                    return true;
                }
            private:
                cytx::log_ptr_t log_;
                int32_t conn_id_;

                const size_t header_length_ = sizeof(header_t);
                boost::array<char, CLIENT_MSG_BUFF_MAX> buff_;
                header_t header_;
                size_t cur_len_ = -1;
                size_t remain_len_ = 0; //half_baked_msg
                size_t cur_index_ = 0;
                std::vector<msg_ptr> msg_list_;
                bool is_valid_ = true;
            };
        }

        struct connection_options
        {
            int32_t disconnect_interval;
            bool batch_send_msg;
        };

        template<typename MSG>
        class tcp_connection : public std::enable_shared_from_this<tcp_connection<MSG>>
        {
            using this_t = tcp_connection<MSG>;
        public:
            using msg_t = MSG;
            using header_t = typename msg_t::header_t;
            using msg_ptr = std::shared_ptr<msg_t>;
            using irouter_t = irouter<this_t>;
            using irouter_ptr = irouter_t*;
            using io_service_t = boost::asio::io_service;
            using connection_t = this_t;
            using connection_ptr = std::shared_ptr<connection_t>;
            using socket_t = boost::asio::ip::tcp::socket;
            using endpoint_t = boost::asio::ip::tcp::endpoint;
            using reader_t = detail::connection_reader<msg_t>;
            using ec_t = boost::system::error_code;
            using batch_msg_t = batch_msg<msg_t>;
            using batch_msg_ptr = std::shared_ptr<batch_msg_t>;
            using handler_t = handler_t<msg_ptr>;
            using context_t = context_t;
            using completion_t = completion_t<msg_ptr>;

        public:
            tcp_connection(io_service_t& ios, irouter_ptr router, int32_t conn_id, const connection_options& options)
                : ios_(ios)
                , log_(cytx::log::force_get_log("conn"))
                , router_ptr_(router)
                , conn_id_(conn_id)
                , options_(options)
                , socket_(ios_)
                , reader_(log_, conn_id_)
                , timer_mgr_(ios_)
            {
                log_->debug("connection {} created", conn_id_);
            }

            ~tcp_connection()
            {
                stop_timer();
                close();
                log_->debug("connection {} destroy", conn_id_);
            }

            net_result connect(const std::string& host, uint32_t port)
            {
                host_ = host;
                port_ = port;

                if (host_ == "0.0.0.0")
                    host_ = "127.0.0.1";

                if (socket_.is_open())
                {
                    ec_t ec;
                    socket_.close(ec);
                }

                log_->debug("connection {} sync connect tcp://{}:{}", conn_id_, host_, port_);
                ec_t ec;
                ec = socket_.connect(cytx::util::to_tcp_endpoint(host_, port_), ec);
                if (!ec)
                {
                    start();
                }
                return net_result(ec);
            }

            void async_connect(const std::string& host, uint32_t port)
            {
                if (is_running_)
                    return;

                host_ = host;
                port_ = port;

                if (host_ == "0.0.0.0")
                    host_ = "127.0.0.1";

                if (socket_.is_open())
                {
                    ec_t ec;
                    socket_.close(ec);
                }

                log_->debug("connection {} connect tcp://{}:{}", conn_id_, host_, port_);
                socket_.async_connect(cytx::util::to_tcp_endpoint(host_, port_), cytx::bind(&this_t::handle_connect, this->shared_from_this()));
            }

            bool start()
            {
                ec_t ec;
                remote_ = socket_.remote_endpoint(ec);
                if (ec)
                {
                    log_->error("connection {} start failed, {}", conn_id_, ec.message());
                    return false;
                }

                reset_state();
                start_read();
                start_timer();
                return true;
            }

            void close()
            {
                if (!force_close_error_)
                {
                    ios_.post(cytx::bind(&this_t::do_close, this->shared_from_this()));
                }
            }

            void write(msg_ptr msgp)
            {
                if (!is_running_)
                    return;

                ios_.post(std::bind(&this_t::do_write, this->shared_from_this(), msgp));
            }

            msg_ptr await_write(msg_ptr msgp, context_t ctx)
            {
                if (!is_running_)
                    return nullptr;

                completion_t completion(std::forward<context_t>(ctx));

                ios_.post(std::bind(&this_t::do_await_write, this->shared_from_this(), msgp, completion.handler));

                return completion.result.get();
            }

            io_service_t& get_io_service() { return ios_; }

            socket_t& socket() { return socket_; }

            bool is_running() const { return is_running_; }

            int32_t get_conn_id() const { return conn_id_; }

            world_map& world()
            {
                return world_;
            }

            world_map& world() const
            {
                return world_;
            }

        private:
            void reset_state()
            {
                ec_t ec;
                socket_.set_option(boost::asio::ip::tcp::no_delay(true), ec);
                received_msg_ = false;
                is_running_ = true;
                force_close_error_ = false;
                write_msg_queue_.clear();
                reader_.reset();
                batch_msg_ptr_ = nullptr;
            }

            void start_read()
            {
                async_sequence_read();
            }
            void start_timer()
            {
                int32_t timeout = options_.disconnect_interval;
                if (timeout > 0)
                {
                    timer_mgr_.set_auto_timer(timeout * 1000, cytx::bind(&this_t::handle_timer, this->shared_from_this()));
                }
            }
            void stop_timer()
            {
                timer_mgr_.stop_all_timer();
            }

            void do_close()
            {
                if (force_close_error_)
                    return;

                stop_timer();
                ec_t ec;
                socket_.shutdown(socket_t::shutdown_both, ec);
                socket_.close(ec);
                write_msg_queue_.clear();
                force_close_error_ = true;
                is_running_ = false;

                log_->debug("connection {} close", conn_id_);
            }
            void do_write(msg_ptr msgp)
            {
                if (!is_running_)
                    return;

                bool write_in_progress = !(write_msg_queue_.empty() && batch_msg_ptr_ == nullptr);
                write_msg_queue_.push_back(msgp);

                if (write_in_progress)
                    return;

                async_write_msg();
            }
            void do_await_write(msg_ptr msgp, handler_t handler)
            {
                if (!is_running_)
                    return;

                uint32_t& call_id = msgp->header().call_id;
                if (call_id <= 0)
                {
                    call_id = ++cur_call_id_;
                }
                calls_.emplace(call_id, handler);

                do_write(msgp);
            }

            void handle_connect(const ec_t& err)
            {
                log_->debug("connection {} connect tcp://{}:{}, result {}", conn_id_, host_, port_, err.message());
                start();
                auto conn = this->shared_from_this();
                router_ptr_->on_connect(conn, err);
            }
            void handle_write(const ec_t& err, msg_ptr msgp)
            {
                if (!is_running_)
                    return;

                if (err)
                {
                    on_error(err, cytx::error_code::send_error);
                    return;
                }

                msgp->ntoh();
                write_msg_queue_.pop_front();

                async_write_msg();
            }
            void handle_write_batch(const ec_t& err)
            {
                if (!is_running_)
                    return;

                if (err)
                {
                    on_error(err, cytx::error_code::send_error);
                    return;
                }

                batch_msg_ptr_ = nullptr;

                async_write_msg();
            }
            void handle_timer()
            {
                log_->trace("connection {} handle timer, received msg {}", conn_id_, received_msg_);
                if (!received_msg_)
                {
                    on_error(cytx::error_code::timeout);
                }
            }

            void async_write_msg()
            {
                if (write_msg_queue_.empty())
                    return;

                using namespace boost::asio;

                if (!options_.batch_send_msg || write_msg_queue_.size() == 1)
                {
                    log_->trace("connection {} write msg, queue size {}", conn_id_, write_msg_queue_.size());
                    msg_ptr msgp = write_msg_queue_.front();
                    msgp->hton();
                    async_write(socket_, msgp->to_buffers(), boost::bind(&this_t::handle_write, this->shared_from_this(), boost::asio::placeholders::error, msgp));
                }
                //批量发送消息
                else
                {
                    batch_msg_ptr_ = std::make_shared<batch_msg_t>();
                    while (!write_msg_queue_.empty())
                    {
                        msg_ptr msgp = write_msg_queue_.front();
                        if (batch_msg_ptr_->add_msg(msgp))
                        {
                            write_msg_queue_.pop_front();
                        }
                        else
                        {
                            break;
                        }
                    }
                    if (batch_msg_ptr_->empty())
                    {
                        on_error(cytx::error_code::send_error);
                        return;
                    }

                    log_->debug("connection {} batch write msg, count {}, length {}, full {}, length full {}", conn_id_, batch_msg_ptr_->size(), batch_msg_ptr_->total_length(),
                        batch_msg_ptr_->full(), batch_msg_ptr_->length_full());

                    batch_msg_ptr_->hton();
                    async_write(socket_, batch_msg_ptr_->to_buffers(), boost::bind(&this_t::handle_write_batch, this->shared_from_this(), boost::asio::placeholders::error));
                }
            }

            void async_sequence_read()
            {
                if (!is_running_)
                    return;

                using namespace boost::asio;

                auto recv_buff = reader_.prepare_next_recv();
                if (buffer_size(recv_buff) > 0)
                {
                    received_msg_ = true;

                    log_->trace("connection {} begin sequence read", conn_id_);
                    async_read(socket_, recv_buff, cytx::bind(&this_t::completion_condition, this->shared_from_this()),
                        cytx::bind(&this_t::handle_sequence_read, this->shared_from_this()));
                }
            }
            void handle_sequence_read(const ec_t& err, size_t bytes_transfered)
            {
                if (err || bytes_transfered == 0 || !reader_.is_valid())
                {
                    on_error(err, cytx::error_code::recv_error);
                    return;
                }

                log_->trace("connection {} handle read, bytes {}", conn_id_, bytes_transfered);
                bool has_msg = reader_.parse(bytes_transfered);
                if (has_msg)
                {
                    on_receive_msgs<header_t>(reader_.get_msgs());
                }
                else
                {
                    reader_.reset();
                }
                async_sequence_read();
            }
            size_t completion_condition(const ec_t& err, size_t bytes_transfered)
            {
                return reader_.completion_condition(err, bytes_transfered);
            }

            void on_error(const cytx::net_result& err)
            {
                if (!is_running_)
                    return;

                log_->debug("connection {} on error {}", conn_id_, err.message());

                is_running_ = false;
                timer_mgr_.stop_all_timer();

                auto conn = this->shared_from_this();
                router_ptr_->on_disconnect(conn, err);
            }
            void on_error(const ec_t& ec, cytx::error_code err)
            {
                on_error(net_result(err, ec));
            }

            template<typename T>
            auto on_receive_msgs(const std::vector<msg_ptr>& msg_list) -> std::enable_if_t<has_call_id_v<T>>
            {
                size_t msg_list_size = msg_list.size();
                size_t new_list_size = 0;
                log_->debug("connection {} start batch process msg, count {}", conn_id_, msg_list_size);

                std::vector<msg_ptr> new_msg_list;
                auto conn_ptr = this->shared_from_this();
                for (auto& msgp : msg_list)
                {
                    received_msg_ = true;
                    uint32_t call_id = msgp->header().call_id;
                    if (call_id > 0)
                    {
                        auto it = calls_.find(call_id);
                        if (it != calls_.end())
                        {
                            handler_t handler = it->second;
                            calls_.erase(it);

                            using boost::asio::asio_handler_invoke;
                            asio_handler_invoke(std::bind(handler, msgp), &handler);
                            continue;
                        }
                    }

                    new_msg_list.push_back(msgp);
                }

                new_list_size = new_msg_list.size();
                if (!new_msg_list.empty())
                {
                    received_msg_ = true;
                    router_ptr_->on_receive_msgs(conn_ptr, new_msg_list);
                }

                log_->debug("connection {} end batch process msg, call count {}, msg count {}", conn_id_, msg_list_size - new_list_size, new_list_size);
            }

            template<typename T>
            auto on_receive_msgs(const std::vector<msg_ptr>& msg_list) -> std::enable_if_t<!has_call_id_v<T>>
            {
                size_t msg_list_size = msg_list.size();
                log_->debug("connection {} start batch process msg, count {}", conn_id_, msg_list_size);

                if (!msg_list.empty())
                {
                    auto conn_ptr = this->shared_from_this();
                    received_msg_ = true;
                    router_ptr_->on_receive_msgs(conn_ptr, msg_list);
                }

                log_->debug("connection {} end batch process msg, msg count {}", conn_id_, msg_list_size);
            }
        private:
            io_service_t& ios_;
            int32_t conn_id_ = 0;
            bool received_msg_ = false;
            cytx::log_ptr_t log_;

            std::atomic<bool> is_running_{ false };
            std::atomic<bool> force_close_error_{ false };
            irouter_ptr router_ptr_ = nullptr;
            connection_options options_;

            socket_t socket_;
            std::string host_;
            uint32_t port_ = 0;
            endpoint_t remote_;

            std::deque<msg_ptr> write_msg_queue_;
            batch_msg_ptr batch_msg_ptr_;
            reader_t reader_;

            timer_manager timer_mgr_;
            uint32_t cur_call_id_ = 0;
            std::map<uint32_t, handler_t> calls_;

            world_map world_;
        };
    }
}
