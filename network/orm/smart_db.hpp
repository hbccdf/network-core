#pragma once
#include "mysql_db.hpp"

namespace cytx
{
    namespace orm
    {
        class smart_db;

        template<typename T>
        class db_proxy
        {
            using parent_t = T;
            friend class smart_db;
        public:
            db_proxy(db_proxy&& db)
                : db_(db.db_)
                , sql_(std::move(db.sql_))
                , sql_where_(std::move(db.sql_where_))
            {
                db.db_ = nullptr;
            }
            ~db_proxy() {}
            parent_t&& where(const expr& where_expr) &&
            {
                if (sql_where_.empty())
                    sql_where_ = fmt::format("WHERE ({})", where_expr.str(true));
                else
                    sql_where_ = fmt::format("{} or ({})", sql_where_, where_expr.str(true));
                return std::move(*(parent_t*)this);
            }
        protected:
            db_proxy(smart_db* db, std::string sql)
                : db_(db)
                , sql_(sql)
            {
            }

            smart_db* db_;
            std::string sql_;
            std::string sql_where_;
        };

        class update_proxy : public db_proxy<update_proxy>
        {
            friend class smart_db;
        public:
            using db_proxy::db_proxy;

            int64_t to_value() && ;
        };

        class delete_proxy : public db_proxy<delete_proxy>
        {
            friend class smart_db;
        public:
            using db_proxy::db_proxy;

            int64_t to_value() && ;
        };

        template<typename RESULT_T>
        class query_proxy
        {
            using result_t = typename RESULT_T::value_type;
            friend class smart_db;
        public:
            query_proxy(query_proxy&& q)
                : db_(q.db_)
                , sql_from_(std::move(sql_from_))
                , sql_select_(std::move(sql_select_))
                , sql_target_(std::move(sql_target_))
                , sql_where_(std::move(sql_where_))
                , sql_groupby_(std::move(sql_groupby_))
                , sql_having_(std::move(sql_having_))
                , sql_orderby_(std::move(sql_orderby_))
                , sql_limit_(std::move(sql_limit_))
                , sql_offset_(std::move(sql_offset_))
            {
            }

            inline query_proxy&& distinct() &&
            {
                sql_select_ = "SELECT DISTINCT";
                return std::move(*this);
            }

            query_proxy&& where(const expr& where_expr) &&
            {
                if (sql_where_.empty())
                    sql_where_ = fmt::format("WHERE ({})", where_expr.str(true));
                else
                    sql_where_ = fmt::format("{} or ({})", sql_where_, where_expr.str(true));
                return std::move(*this);
            }

            template <typename P, typename T>
            inline query_proxy&& groupby(const field_proxy<P, T>& field) &&
            {
                sql_groupby_ = fmt::format(" group by {}.{}", field.table_name(), field.name());
                return std::move(*this);
            }

            inline query_proxy&& having(const expr& having_expr) &&
            {
                sql_having_ = " having " + having_expr.str(true);
                return std::move(*this);
            }

            inline query_proxy&& take(size_t count) &&
            {
                sql_limit_ = fmt::format(" limit {}", count);
                return std::move(*this);
            }

            inline query_proxy&& Skip(size_t count) &&
            {
                if (sql_limit_.empty())
                    sql_limit_ = " limit ~0";  // ~0 is a trick :-)
                sql_offset_ = fmt::format(" offset {}", count);
                return std::move(*this);
            }

            template <typename P, typename T>
            inline query_proxy&& orderby(const field_proxy<P, T> &field) &&
            {
                if (sql_orderby_.empty())
                    sql_orderby_ = fmt::format(" order by {}.{}", field.table_name(), field.name());
                else
                    sql_orderby_ = fmt::format("{}, {}.{}", sql_orderby_, field.table_name(), field.name());
                return std::move(*this);
            }

            template <typename P, typename T>
            inline query_proxy&& orderby_desc(const field_proxy<P, T> &field) &&
            {
                if (sql_orderby_.empty())
                    sql_orderby_ = fmt::format(" order by {}.{} desc", field.table_name(), field.name());
                else
                    sql_orderby_ = fmt::format("{}, {}.{} desc", sql_orderby_, field.table_name(), field.name());
                return std::move(*this);
            }

        public:
            std::vector<result_t> to_vector() && ;

        protected:
            query_proxy(smart_db* db, std::string sql_from, std::string sql_target = "*")
                : db_(db)
                , sql_from_(std::move(sql_from))
                , sql_target_(std::move(sql_target))
            {
            }

            inline std::string get_from_sql() const
            {
                return fmt::format("{}{}{}{}", sql_from_, sql_where_, sql_groupby_, sql_having_);
            }

            // Return ORDER BY & LIMIT part for Query
            inline std::string get_limit() const
            {
                return fmt::format("{}{}{}", sql_orderby_, sql_limit_, sql_offset_);
            }


        protected:
            smart_db* db_;

            std::string sql_from_ = " FROM ";
            std::string sql_select_ = "SELECT ";
            std::string sql_target_ = "*";

            std::string sql_where_;
            std::string sql_groupby_;
            std::string sql_having_;

            std::string sql_orderby_;
            std::string sql_limit_;
            std::string sql_offset_;
        };

        class smart_db : public mysql_db
        {
        public:
            using mysql_db::mysql_db;

            template<typename T>
            auto insert(T&& t) -> std::enable_if_t<is_reflection<std::decay_t<T>>::value>
            {
                fmt::MemoryWriter names_wr;
                fmt::MemoryWriter values_wr;
                bool is_first = true;

                for_each(std::forward<T>(t), [&names_wr, &values_wr, &is_first](auto& item, size_t I, bool is_last)
                {
                    if (!item.second)
                        return;

                    names_wr.write("{}{}", is_first ? "" : ",", item.first);
                    values_wr.write("{}{}", is_first ? "" : ",", util::cast_string(item.second.value()));
                    is_first = false;
                });

                string insert_sql = fmt::format("INSERT INTO `{}`({}) values({})", get_name<T>(), names_wr.str(), values_wr.str());
                execute_general(insert_sql);
            }

            template<typename Arg0, typename ... Args>
            auto insert(Arg0&& arg0, Args&& ... args) -> std::enable_if_t<std::is_base_of<set_expr, typename std::decay_t<Arg0>>::value>
            {
                fmt::MemoryWriter names_wr;
                fmt::MemoryWriter values_wr;
                bool is_first = true;

                auto t = std::make_tuple(arg0, args...);
                for_each(t, [&names_wr, &values_wr, &is_first](auto& item, size_t I, bool is_last)
                {
                    if (!item)
                        return;

                    names_wr.write("{}{}", is_first ? "" : ",", item.name());
                    values_wr.write("{}{}", is_first ? "" : ",", util::cast_string(item.value()));
                    is_first = false;
                });
                string insert_sql = fmt::format("INSERT INTO `{}`({}) values({})", arg0.table_name(), names_wr.str(), values_wr.str());
                execute_general(insert_sql);
            }

            template<typename T>
            auto update(T&& t) -> std::enable_if_t<is_reflection<std::decay_t<T>>::value, update_proxy>
            {
                fmt::MemoryWriter values_wr;
                bool is_first = true;

                for_each(t, [&values_wr, &is_first](auto& item, size_t I, bool is_last)
                {
                    if (!item.second)
                        return;

                    values_wr.write("{}={}", item.first, util::cast_string(item.second.value()), is_last ? "" : ",");
                    is_first = false;
                });

                string update_sql = fmt::format("UPDATE `{}` SET {}", std::decay_t<T>::type_name(), values_wr.str());
                return update_proxy(this, update_sql);
            }

            template<typename Arg0, typename ... Args>
            auto update(Arg0&& arg0, Args&& ... args) -> std::enable_if_t<std::is_base_of<set_expr, typename std::decay_t<Arg0>>::value, update_proxy>
            {
                auto t = std::make_tuple(arg0, args...);
                fmt::MemoryWriter values_wr;
                bool is_first = true;

                for_each(t, [&values_wr, &is_first](auto& item, size_t I, bool is_last)
                {
                    values_wr.write("{}{}", is_first ? "" : ",", item.str());
                    is_first = false;
                });

                string update_sql = fmt::format("UPDATE `{}` SET {}", arg0.table_name(), values_wr.str());
                return update_proxy(this, update_sql);
            }

            template<typename T>
            auto delete_item()
                -> std::enable_if_t<is_reflection<std::decay_t<T>>::value, delete_proxy>
            {
                string delete_sql = fmt::format("DELETE FROM `{}`", get_name<T>());
                return delete_proxy(this, delete_sql);
            }

            template<typename T>
            auto query() -> std::enable_if_t<is_reflection<std::decay_t<T>>::value, query_proxy<T>>
            {
                return query_proxy<T>(this, fmt::format(" from {} ", get_name<T>()));
            }
        };

        inline int64_t update_proxy::to_value() &&
        {
            std::string update_sql = fmt::format("{} {}", sql_, sql_where_);
            return db_->execute_general(update_sql);
        }

        inline int64_t delete_proxy::to_value() &&
        {
            std::string delete_sql = fmt::format("{} {}", sql_, sql_where_);
            return db_->execute_general(delete_sql);
        }

        template<typename RESULT_T>
        std::vector<typename RESULT_T::value_type> cytx::orm::query_proxy<RESULT_T>::to_vector() &&
        {
            std::string sql = fmt::format("{}{}{}{};", sql_select_, sql_target_, get_from_sql(), get_limit());
            auto result_set = db_->execute_query(sql);

            std::vector<result_t> v;
            for (size_t i = 0; i < result_set.row_size(); ++i)
            {
                RESULT_T t;
                auto row = result_set[i];
                for_each(t, [this, &row](auto& item, size_t I, bool is_last) {
                    auto str = row[item.first];
                    item.second.set_value(util::cast<std::decay_t<decltype(item.second.value())>>(str));
                });
                v.push_back(t.Value());
            }
            return v;
        }
    }
}