#pragma once
#include "../base/utils.hpp"
#include "server.hpp"
#include "server_config.hpp"
#include "msg_pack.hpp"

namespace cytx
{
    namespace gameserver
    {
        namespace detail
        {
            using namespace cytx;
            using namespace cytx::gameserver;

            using msg_t = server_msg<msg_body>;
            using header_t = typename msg_t::header_t;
            using server_t = tcp_server<msg_t>;
            using server_ptr = std::unique_ptr<server_t>;
            using connection_t = typename server_t::connection_t;
            using connection_ptr = typename server_t::connection_ptr;
            using msg_ptr = typename server_t::msg_ptr;
            using irouter_t = typename server_t::irouter_t;
            using irouter_ptr = typename server_t::irouter_ptr;
            using timer_manager_t = timer_manager;
            using timer_manager_ptr = std::unique_ptr<timer_manager_t>;
            using timer_t = timer_proxy;

            class game_server : public irouter_t
            {
                using this_t = game_server;
            public:
                game_server(server_unique_id unique_id)
                    : unique_id_(unique_id)
                {

                }
                void init(const std::string& config_file_name = "server_config.xml")
                {
                    //��ʼ���ڴ��
                    MemoryPoolManager::get_mutable_instance().init();
                    //��ȡ�����ļ�
                    config_mgr_.init(config_file_name);
                    config_mgr_.parse_real_ip(cytx::util::domain2ip);

                    //��ʼ����־
                    const server_info& info = config_mgr_[unique_id_];
                    for (auto& log_cfg : info.logs)
                    {
                        auto log_ptr = log::init_log(fmt::format("logs/{}", log_cfg.filename), log_cfg.level, log_cfg.name, log_cfg.console);
                        logs_.push_back(log_ptr);
                    }

                    //��ʼ��server��timer
                    server_options options;
                    options.thread_mode = (cytx::gameserver::server_thread_mode)info.thread_mode;
                    options.disconnect_interval = info.disconnect_heartbeat;
                    options.batch_send_msg = info.batch_send_msg;

                    server_ = std::make_unique<server_t>(info.ip, info.port, options, this);
                    timer_mgr_ = std::make_unique<timer_manager_t>(server_->get_io_service());
                    flush_log_timer_ = timer_mgr_->set_auto_timer(info.flush_log_time, cytx::bind(&this_t::flush_logs, this));

                    //���Ӹ�������
                }

                void start()
                {
                    flush_log_timer_.start();
                    server_->start();
                }
                void stop()
                {
                    timer_mgr_->stop_all_timer();
                    server_->stop();

                    flush_logs();
                }

            public:
                //������˷�����Ϣ
                template<typename MSG_ID, typename T>
                void send_msg(server_unique_id unique_id, MSG_ID msg_id, const T& t)
                {
                    connection_ptr conn_ptr = get_connection_ptr(unique_id);
                    if (!conn_ptr)
                    {
                        //TODO log error
                        return;
                    }

                    msg_ptr msgp = pack_msg(t, header_t::big_endian());
                    auto& header = msgp->header();
                    header.protocol_id = (uint32_t)msg_id;
                    header.from_unique_id = (uint16_t)unique_id_;
                    header.to_unique_id = (uint16_t)unique_id;

                    send_raw_msg(conn_ptr, msgp);
                }

                template<typename MSG_ID, typename T>
                void send_client_msg(int32_t user_id, MSG_ID msg_id, const T& t)
                {
                    connection_ptr conn_ptr = gate_conn_ptr_;
                    if (!conn_ptr)
                    {
                        //TODO log error
                        return;
                    }

                    msg_ptr msgp = pack_msg(t, header_t::big_endian());
                    auto& header = msgp->header();
                    header.protocol_id = (uint32_t)msg_id;
                    header.from_unique_id = (uint16_t)unique_id_;
                    header.to_unique_id = (uint16_t)server_unique_id::gateway_server;
                    header.user_id = user_id;

                    send_raw_msg(conn_ptr, msgp);
                }

                template<typename MSG_ID, typename T>
                void broadcast_client_msg(const std::vector<int32_t>& users, MSG_ID msg_id, const T& t)
                {
                    const uint32_t broadcast_msg_id = 0x60a001a;

                    connection_ptr conn_ptr = gate_conn_ptr_;
                    if (!conn_ptr)
                    {
                        //TODO log error
                        return;
                    }

                    msg_ptr msgp = pack_msg(header_t::big_endian(), (uint32_t)msg_id, users, t);
                    auto& header = msgp->header();
                    header.protocol_id = broadcast_msg_id;
                    header.from_unique_id = (uint16_t)unique_id_;
                    header.to_unique_id = (uint16_t)server_unique_id::gateway_server;

                    send_raw_msg(conn_ptr, msgp);
                }
            private:
                void on_connect(connection_ptr& conn_ptr, const net_result& err)
                {

                }
                void on_disconnect(connection_ptr& conn_ptr, const net_result& err)
                {

                }
                void on_receive(connection_ptr& conn_ptr, const msg_ptr& msgp)
                {

                }

                void send_raw_msg(connection_ptr& conn_ptr, msg_ptr& msgp)
                {
                    conn_ptr->write(msgp);
                }

                connection_ptr get_connection_ptr(server_unique_id unique_id) const
                {
                    if (unique_id == server_unique_id::gateway_server)
                        return gate_conn_ptr_;
                    else if (unique_id == server_unique_id::db_server)
                        return db_conn_ptr_;
                    else
                        return server_mgr_conn_ptr_;
                }

            private:
                void flush_logs()
                {
                    for (auto& log_ptr : logs_)
                    {
                        log_ptr->flush();
                    }
                }
            private:
                server_unique_id unique_id_;
                server_config_manager config_mgr_;
                server_ptr server_;
                timer_manager_ptr timer_mgr_;
                std::vector<log_ptr_t> logs_;
                timer_t flush_log_timer_;

                connection_ptr gate_conn_ptr_;
                connection_ptr db_conn_ptr_;
                connection_ptr server_mgr_conn_ptr_;
                //server_manager����������
                //DB����������
                //��������ȷ��
            };
        }
    }
}