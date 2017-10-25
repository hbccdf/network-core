#pragma once
#include <memory>
#include <queue>
#include <string>
#include <atomic>
#include <boost/asio.hpp>
#include <fmt/format.h>
#include "../base/log.hpp"
#include "../base/utils.hpp"
#include "../timer/timer_manager.hpp"
#include "raw_msg.hpp"
#include "irouter_base.hpp"

#define CLIENT_MSG_BUFF_MAX 1024 * 100 //100K

namespace cytx
{
    namespace gameserver
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
                    assert(data_len <= CLIENT_MSG_BUFF_MAX);

                    if ((size_t)-1 == cur_len_ && data_len >= header_length_)
                    {
                        parse_header(buff_.begin());
                    }

                    return data_len >= cur_len_ ? 0 : boost::asio::detail::default_max_transfer_size;
                }

                bool parse(size_t bytes_transfered)
                {
                    remain_len_ += bytes_transfered;
                    auto pnext = buff_.begin();
                    assert(remain_len_ <= CLIENT_MSG_BUFF_MAX);

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

                    return parse_ok;
                }

                void reset()
                {
                    cur_len_ = -1;
                    remain_len_ = 0;
                    cur_index_ = 0;
                    header_.length = 0;
                    msg_list_.clear();
                }

                const std::vector<msg_ptr>& get_msgs() const { return msg_list_; }

            private:
                template<typename T>
                bool parse_header(const void* src)
                {
                    memcpy(&header_, src, header_length_);
                    header_.ntoh();
                    cur_len_ = header_length_ + header_.length;

                    if (cur_len_ > CLIENT_MSG_BUFF_MAX || cur_len_ < header_length_)
                    {
                        LOG_ERROR("completion_condition: {}", cur_len_);
                        return false;
                    }
                    return true;
                }
            private:
                const size_t header_length_ = sizeof(header_t);
                std::array<char, CLIENT_MSG_BUFF_MAX> buff_;
                header_t header_;
                size_t cur_len_ = 0;
                size_t remain_len_ = 0; //half_baked_msg
                size_t cur_index_ = 0;
                std::vector<msg_ptr> msg_list_;
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
            using connection_ptr = std::shared_ptr<connection_ptr>;
            using socket_t = boost::asio::ip::tcp::socket;
            using endpoint_t = boost::asio::ip::tcp::endpoint;
            using reader_t = connection_reader<msg_t>;
            using ec_t = boost::system::error_code;
            using batch_msg_t = batch_msg<msg_t>;
            using batch_msg_ptr = std::shared_ptr<batch_msg_t>;

        public:
            tcp_connection(io_service_t& ios, irouter_ptr router, int32_t conn_id, const connection_options& options)
                : ios_(ios)
                , router_ptr_(router)
                , conn_id_(conn_id)
                , options_(options)
                , timer_mgr_(ios_)
            {

            }

            ~tcp_connection()
            {
                stop_timer();
                LOG_DEBUG("connection {} destruct", conn_id_);
            }

            void async_connect(const std::string& host, uint32_t port)
            {
                host_ = host;
                port_ = port;

                using namespace boost::asio;
                using namespace boost::asio::ip;
                socket_.async_connect(cytx::util::get_tcp_endpoint(host_, port_), cytx::bind(&this_t::handle_connect, shared_from_this()));
            }

            bool start()
            {
                ec_t ec;
                remote_ = socket_.remote_endpoint(ec);
                if (ec)
                {
                    LOG_ERROR("start failed, {}", ec.message());
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
                    ios_.post(cytx::bind(&this_t::do_close, shared_from_this()));
                }
            }

            void write(msg_ptr msgp)
            {
                if (!is_running_)
                    return;

                ios_.post(cytx::bind(&this_t::do_write, shared_from_this(), msgp));
            }

        private:
            void reset_state()
            {
                ec_t ec;
                socket_.set_option(boost::asio::ip::tcp::no_delay(true), ec);
                is_running_ = false;
                force_close_error_ = false;
                write_msg_queue_.clear();
                received_msg_ = false;
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
                    timer_mgr_.set_auto_timer(timeout * 1000, cytx::bind(&this_t::handle_timer, shared_from_this()));
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
                socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
                socket_.close(ec);
                write_msg_queue_.clear();
                force_close_error_ = true;
                is_running_ = false;

                LOG_DEBUG("connect close {}", conn_id_);
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

            void handle_connect(const ec_t& err)
            {
                router_ptr_->connection_incoming(shared_from_this(), err);
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
                if (!receive_msg_)
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
                    msg_ptr msgp = write_msg_queue_.front();
                    msgp->hton();
                    async_write(socket_, msgp->to_buffers(), boost::bind(&this_t::handle_write, shared_from_this(), placeholders::error, msgp));
                }
                //批量发送消息
                else
                {
                    batch_msg_ptr_ = std::make_shared<batch_msg_t>();
                    while (true)
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

                    batch_msg_ptr_->hton();
                    async_write(socket_, batch_msg_ptr_->to_buffers(), boost::bind(&this_t::handle_write_batch, shared_from_this(), placeholders::error));
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
                    async_read(socket_, recv_buff, cytx::bind(&this_t::completion_condition, shared_from_this()),
                        cytx::bind(&this_t::handle_sequence_read, shared_from_this()));
                }
            }
            void handle_sequence_read(const ec_t& err, size_t bytes_transfered)
            {
                if (err || bytes_transfered == 0)
                {
                    on_error(err, cytx::error_code::recv_error);
                    return;
                }

                bool has_msg = reader_.parse(bytes_transfered);
                if (has_msg)
                {
                    router_ptr_->message_received(shared_from_this(), reader_.get_msgs());
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

                is_running_ = false;
                router_ptr_.connection_terminated(shared_from_this(), err);
            }
            void on_error(const ec_t& ec, cytx::error_code)
            {
                on_error(net_result(err, ec));
            }
        private:
            io_service_t& ios_;
            int32_t conn_id_ = 0;
            bool received_msg_ = false;

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
        };
    }
}
