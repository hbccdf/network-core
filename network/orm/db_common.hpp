#pragma once
#include "db_meta.hpp"
#include <my_global.h>
#include <mysql.h>
#include <string>
#include <vector>

namespace cytx
{
    namespace orm
    {
        class mysql_exception;
        class mysql_result
        {
            friend class mysql_exception;
        public:
            mysql_result() noexcept
                : ec_(0)
                , msg_()
            {
            }

            mysql_result(unsigned int ec)
                : ec_(ec)
                , msg_()
            {
            }

            mysql_result(unsigned int ec, std::string em)
                : ec_(ec)
                , msg_(std::move(em))
            {
            }

            mysql_result(MYSQL* conn)
                : ec_(mysql_errno(conn))
                , msg_(mysql_error(conn))
            {

            }

            explicit operator bool() const noexcept
            {
                return !is_success();
            }

            bool operator!() const noexcept
            {
                return is_success();
            }

            bool is_success() const noexcept
            {
                return ec_ == 0;
            }

            unsigned int code() const { return ec_; }

            std::string message() const
            {
                return fmt::format("{}, {}", ec_, msg_);
            }

            std::string raw_msg() const noexcept { return msg_; }
        protected:
            int ec_;
            std::string msg_;
        };

        class mysql_exception : public mysql_result
        {
        public:
            using mysql_result::mysql_result;

            mysql_exception(mysql_result& r)
                : mysql_result(r)
            {}

            mysql_exception& operator= (mysql_result& r)
            {
                this->ec_ = r.ec_;
                this->msg_ = r.msg_;
                return *this;
            }

            mysql_result result()
            {
                mysql_result r;
                r.ec_ = ec_;
                r.msg_ = msg_;
                return r;
            }
        };

        class mysql_result_set;
        class mysql_db;

        class mysql_row
        {
            friend class mysql_result_set;
        public:
            mysql_row(const mysql_row& o) = default;

            mysql_row(mysql_row&& o)
                : fields_ptr_(o.fields_ptr_)
                , row_(o.row_)
            {
                o.fields_ptr_ = nullptr;
                o.row_ = nullptr;
            }

            std::string operator[](size_t index)
            {
                return row_[index];
            }

            std::string operator[](const std::string& key)
            {
                auto it = fields_ptr_->find(key);
                auto index = it->second;
                return row_[index];
            }

            size_t size() { return fields_ptr_->size(); }
        private:
            mysql_row(std::map<std::string, int>* fields_ptr, MYSQL_ROW row)
                : fields_ptr_(fields_ptr)
                , row_(row)
            {
            }
            std::map<std::string, int>* fields_ptr_ = nullptr;
            MYSQL_ROW row_ = nullptr;
        };

        class mysql_result_set
        {
            friend class mysql_db;
        public:
            mysql_result_set(mysql_result_set&& o)
                : fields_map_(std::move(o.fields_map_))
                , rows_(std::move(o.rows_))
                , fields_(std::move(o.fields_))
                , res_(o.res_)
                , conn_(o.conn_)
            {
                for (auto& r : rows_)
                {
                    r.fields_ptr_ = &fields_map_;
                }

                o.res_ = nullptr;
                o.conn_ = nullptr;
            }
            ~mysql_result_set()
            {
                if (res_)
                    mysql_free_result(res_);
            }

            void init()
            {
                res_ = mysql_store_result(conn_);

                int field_num = mysql_num_fields(res_);
                MYSQL_FIELD* fields = mysql_fetch_fields(res_);
                fields_.resize(field_num);
                memcpy(fields_.data(), fields, sizeof(MYSQL_FIELD) * field_num);

                for (int i = 0; i < field_num; ++i)
                {
                    fields_map_[fields[i].name] = i;
                }

                auto row = mysql_fetch_row(res_);
                while (row)
                {
                    rows_.push_back(mysql_row(&fields_map_, row));
                    row = mysql_fetch_row(res_);
                }
            }

            mysql_row operator[](size_t index)
            {
                return rows_[index];
            }

            size_t row_size() const { return rows_.size(); }
            size_t column_size() const { return fields_map_.size(); }

            std::string column_name(size_t index) const { return fields_[index].name; }

        private:
            mysql_result_set(MYSQL* conn)
                : conn_(conn)
            {}
        private:
            std::map<std::string, int> fields_map_;
            std::vector<mysql_row> rows_;
            std::vector<MYSQL_FIELD> fields_;
            MYSQL_RES* res_ = nullptr;
            MYSQL* conn_ = nullptr;
        };
    }
}