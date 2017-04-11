#pragma once
#include "db_common.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

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
                register_table(T::type_name(), create_table_sql);
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
            mysql_db(std::string connect_str)
            {
                conn_ = mysql_init(conn_);
                connect_str_ = connect_str;
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
                    mr = mysql_result();
                    return;
                }
                auto args = detail::split(connect_str_);
                ip_ = args["ip"];
                port_ = boost::lexical_cast<uint32_t>(args["port"]);
                user_ = args["user"];
                pwd_ = args["pwd"];

                if (mysql_real_connect(conn_, ip_.c_str(), user_.c_str(), pwd_.c_str(), nullptr, port_, nullptr, 0) == nullptr)
                {
                    mr = mysql_result(conn_);
                }
                else
                {
                    is_connected_ = true;
                    mr = mysql_result();
                }
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

                if (use_new_db && mysql_select_db(conn_, db_name.c_str()) != 0)
                    mr = mysql_result(conn_);
                else
                    mr = mysql_result();
            }

            template<typename T>
            void create_table()
            {
                std::string sql = table_factory::instance().create_table_sql_map_[T::type_name()];
                execute(sql);
            }

            template<typename T>
            void drop_table()
            {
                execute(fmt::format("DROP TABLE IF EXISTS {}", T::type_name()));
            }

            void execute(std::string sql_str)
            {
                mysql_result mr;
                execute(sql_str, mr);
                if (mr)
                    throw mysql_exception(mr);
            }

            void execute(std::string sql_str, mysql_result& mr)
            {
                if (mysql_query(conn_, sql_str.c_str()))
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
                if (mysql_query(conn_, sql_str.c_str()))
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
                T t;
                if (mysql_query(conn_, sql_str.c_str()))
                {
                    mr = mysql_result(conn_);
                    return t;
                }
                MYSQL_RES* res = mysql_store_result(conn_);
                MYSQL_ROW  row = mysql_fetch_row(res);
                t = boost::lexical_cast<T>(row[0]);
                mysql_free_result(res);
                return t;
            }

            mysql_result_set execute_query(std::string sql_str)
            {
                if (mysql_query(conn_, sql_str.c_str()))
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

        protected:
            MYSQL* conn_ = nullptr;
            bool is_connected_ = false;
            std::string connect_str_;
            std::string ip_;
            uint32_t port_;
            std::string user_;
            std::string pwd_;
        };
    }
}