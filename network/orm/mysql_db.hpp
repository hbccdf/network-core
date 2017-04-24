#pragma once
#include "db_common.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "../base/log.hpp"
#include <errmsg.h>

namespace cytx
{
    namespace orm
    {
        namespace detail
        {
            inline std::map<std::string, std::string> split(std::string connect_str)
            {
                std::map<std::string, std::string> connect_args;

                std::vector<std::string> tmp_strs;
                boost::split(tmp_strs, connect_str, boost::is_any_of(";"), boost::token_compress_on);
                for (auto tmp_str : tmp_strs)
                {
                    std::vector<std::string> pairs;
                    boost::split(pairs, tmp_str, boost::is_any_of("="), boost::token_compress_on);
                    if (pairs.size() != 2)
                        continue;
                    connect_args[pairs[0]] = pairs[1];
                }
                return std::move(connect_args);
            }
        }

        using log_ptr_t = std::shared_ptr<spdlog::logger>;

        class mysql_db;
        class table_factory
        {
            friend class mysql_db;
        public:
            static table_factory& instance()
            {
                static table_factory tf;
                return tf;
            }

            template<typename T>
            void register_table(std::string create_table_sql)
            {
                register_table(get_name<T>(), create_table_sql);
            }

            void register_table(std::string table_name, std::string create_table_sql)
            {
                create_table_sql_map_[table_name] = create_table_sql;
            }
        private:
            std::map<std::string, std::string> create_table_sql_map_;
        };

        class mysql_db
        {
        public:
            mysql_db(std::string connect_str, log_ptr_t log = nullptr)
                : log_(log)
            {
                if (log_)
                    log_->debug("---------------------------------------------------------");

                conn_ = mysql_init(conn_);
                connect_str_ = connect_str;
                auto args = detail::split(connect_str_);
                ip_ = args["ip"];
                port_ = boost::lexical_cast<uint32_t>(args["port"]);
                user_ = args["user"];
                pwd_ = args["pwd"];
                
                auto it = args.find("db");
                if (it != args.end())
                {
                    db_name_ = it->second;
                }
            }

            ~mysql_db()
            {
                if (conn_)
                    mysql_close(conn_);
            }

            void connect()
            {
                mysql_result mr;
                connect(mr);
                if (mr)
                    throw mysql_exception(mr);
            }

            void connect(mysql_result& mr)
            {
                if (is_connected_)
                {
                    if (log_)
                        log_->warn("repeat connect");

                    mr = mysql_result();
                    return;
                }

                if (mysql_real_connect(conn_, ip_.c_str(), user_.c_str(), pwd_.c_str(), nullptr, port_, nullptr, 0) == nullptr)
                {
                    if (log_)
                        log_->error("connect mysql failed, code:{}, msg:{}", mysql_errno(conn_), mysql_error(conn_));
                    mr = mysql_result(conn_);
                }
                else
                {
                    if (log_)
                        log_->info("connect mysql success");
                    is_connected_ = true;
                    mr = mysql_result();

                    if (!db_name_.empty())
                    {
                        select_db(db_name_, mr);
                    }
                }
            }

            void select_db(std::string db_name)
            {
                mysql_result mr;
                select_db(db_name, mr);
                if (mr)
                    throw mysql_exception(mr);
            }

            void select_db(std::string db_name, mysql_result& mr)
            {
                db_name_ = db_name;
                if (mysql_select_db(conn_, db_name.c_str()) != 0)
                    mr = mysql_result(conn_);
                else
                    mr = mysql_result();
            }

            void create_db(std::string db_name, bool use_new_db = true)
            {
                mysql_result mr;
                create_db(db_name, mr, use_new_db);
                if (mr)
                    throw mysql_exception(mr);
            }

            void create_db(std::string db_name, mysql_result& mr, bool use_new_db = true)
            {
                execute(fmt::format("CREATE DATABASE IF NOT EXISTS {} DEFAULT CHARSET utf8 COLLATE utf8_general_ci;", db_name), mr);
                if (mr)
                {
                    return;
                }

                if (use_new_db)
                    select_db(db_name, mr);
            }

            template<typename T>
            void create_table()
            {
                std::string sql = table_factory::instance().create_table_sql_map_[get_name<T>()];
                execute_batch(sql);
            }

            template<typename T>
            void drop_table()
            {
                execute(fmt::format("DROP TABLE IF EXISTS {}", get_name<T>()));
            }

            void execute(std::string sql_str)
            {
                mysql_result mr;
                execute(sql_str, mr);
                if (mr)
                    throw mysql_exception(mr);
            }

            void execute_batch(std::string sql_str)
            {
                std::vector<std::string> sqls;
                boost::algorithm::split(sqls, sql_str, boost::is_any_of(";"), boost::algorithm::token_compress_on);
                for (const auto& s : sqls)
                {
                    if (!s.empty())
                    {
                        execute(s);
                    }
                }
            }

            void execute(std::string sql_str, mysql_result& mr)
            {
                if (inter_query(conn_, sql_str.c_str()))
                {
                    mr = mysql_result(conn_);
                }
                else
                {
                    mr = mysql_result();
                }
            }

            int64_t execute_general(std::string sql_str)
            {
                mysql_result mr;
                int64_t r = execute_general(sql_str, mr);
                if (mr)
                    throw mysql_exception(mr);
                return r;
            }

            int64_t execute_general(std::string sql_str, mysql_result& mr)
            {
                if (inter_query(conn_, sql_str.c_str()))
                {
                    mr = mysql_result(conn_);
                    return 0;
                }
                return (int64_t)mysql_affected_rows(conn_);
            }

            template<typename T = int64_t>
            T execute_scalar(std::string sql_str)
            {
                mysql_result mr;
                T t = execute_scalar<T>(sql_str, mr);
                if (mr)
                    throw mysql_exception(mr);
                return t;
            }

            template<typename T = int64_t>
            T execute_scalar(std::string sql_str, mysql_result& mr)
            {
                T t = 0;
                if (inter_query(conn_, sql_str.c_str()))
                {
                    mr = mysql_result(conn_);
                    return t;
                }
                MYSQL_RES* res = mysql_store_result(conn_);
                try
                {
                    MYSQL_ROW  row = mysql_fetch_row(res);
                    t = boost::lexical_cast<T>(row[0]);
                }
                catch (...)
                {

                }
                if (res != nullptr)
                {
                    mysql_free_result(res);
                }
                return t;
            }

            mysql_result_set execute_query(std::string sql_str)
            {
                if (inter_query(conn_, sql_str.c_str()))
                {
                    throw mysql_exception(conn_);
                }
                else
                {
                    mysql_result_set res(conn_);
                    res.init();
                    return res;
                }
            }

            bool is_connect()
            {
                return conn_ != nullptr && is_connected_;
            }

            const log_ptr_t& get_log() const
            {
                return log_;
            }

        protected:
            int inter_query(MYSQL *mysql, const char *q)
            {
                int query_id = ++query_id_;
                if (log_)
                    log_->info("[query_id:{}] {}", query_id_, q);

                if (mysql_query(mysql, q))
                {
                    int err = mysql_errno(mysql);
                    if (err == CR_SERVER_GONE_ERROR || err == CR_SERVER_LOST)
                    {
                        if(log_)
                            log_->warn("[query_id:{}] connect disconnect, code:{}, msg:{}", query_id, err, mysql_error(mysql));
                        //需要重连
                        is_connected_ = false;
                        mysql_close(conn_);
                        conn_ = nullptr;
                        conn_ = mysql_init(conn_);
                        mysql_result mr;
                        connect(mr);
                        if (mr)
                        {
                            if(log_)
                                log_->error("[query_id:{}] reconnect failed, code:{}, msg:{}", query_id, err, mysql_error(mysql));
                            return mr.code();
                        }
                        else
                        {
                            int second_query = mysql_query(mysql, q);
                            if (second_query)
                            {
                                if(log_)
                                    log_->error("[query_id:{}] failed, code:{}, msg:{}", query_id, err, mysql_error(mysql));
                            }
                            else
                            {
                                log_->info("[query_id:{}] success", query_id);
                            }
                            return second_query;
                        }
                    }
                    else
                    {
                        if (log_)
                            log_->error("[query_id:{}] failed, code:{}, msg:{}", query_id, err, mysql_error(mysql));
                        return err;
                    }
                }
                if(log_)
                    log_->debug("[query_id:{}] success", query_id);
                return 0;
            }

        protected:
            MYSQL* conn_ = nullptr;
            bool is_connected_ = false;
            std::string connect_str_;
            log_ptr_t log_ = nullptr;
            std::string ip_;
            uint32_t port_;
            std::string user_;
            std::string pwd_;
            int query_id_ = 0;
            std::string db_name_;
        };
    }
}