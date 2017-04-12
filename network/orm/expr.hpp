#pragma once
#include <fmt/format.h>
#include <boost/optional.hpp>
#include <vector>
#include <map>
#include <memory>
#include <cstdint>
#include "../serialize/common.hpp"
#include "../traits/traits.hpp"

namespace cytx {
    namespace util
    {
        template<typename T>
        auto cast_string(const T& t) -> std::enable_if_t<is_basic_type<T>::value && !std::is_same<std::string, T>::value, std::string>
        {
            return fmt::format("{}", t);
        }

        template<typename T>
        auto cast_string(const T& t) -> std::enable_if_t<std::is_same<std::string, T>::value, std::string>
        {
            return fmt::format("'{}'", t);
        }
    }
    namespace orm
    {
        struct set_expr
        {
            set_expr(std::string field_op_val)
                : expr_{ std::move(field_op_val) }
            {}

            const std::string& str() const
            {
                return expr_;
            }

            inline set_expr operator && (const set_expr &right) const
            {
                return set_expr{ expr_ + "," + right.expr_ };
            }

        protected:
            std::string expr_;
        };

        template <typename T>
        struct select_able
        {
            std::string field_name_;
            const std::string* table_name_;

            select_able(std::string field_name, const std::string* table_name)
                : field_name_(std::move(field_name)), table_name_(table_name)
            {}
        };

        template<typename T>
        struct field_proxy : public select_able<T>, public set_expr
        {
            using value_t = T;

            field_proxy(const std::string* table_name, T* v, std::string name)
                : select_able<T>(name, table_name)
                , set_expr("")
                , val_(v), is_init_(false) {}

            field_proxy& operator=(const field_proxy& o)
            {
                *val_ = *o.val_;
                is_init_ = o.is_init_;
                return *this;
            }

            explicit operator bool() const noexcept
            {
                return is_init_;
            }
            bool operator! ()
            {
                return !is_init_;
            }

            std::string name() const { return field_name_; }
            const std::string& table_name() const { return *table_name_; }

            T value()
            {
                return *val_;
            }

            void set_value(T v)
            {
                is_init_ = true;
                *val_ = v;
            }

        public:
            inline field_proxy& operator = (T v)
            {
                set_value(v);
                auto str_expr = fmt::format("{}.{}={}", table_name(), field_name_, util::cast_string(v));
                expr_ = str_expr;
                return *this;
            }
            inline field_proxy& operator = (set_expr v)
            {
                auto str_expr = fmt::format("{}.{}={}", table_name(), field_name_, v.str());
                expr_ = str_expr;
                return *this;
            }

            inline set_expr operator + (T v) { return add_operator("+", v); }
            inline set_expr operator - (T v) { return add_operator("-", v); }
            inline set_expr operator * (T v) { return add_operator("*", v); }
            inline set_expr operator / (T v) { return add_operator("/", v); }
            inline set_expr operator % (T v) { return add_operator("%", v); }

            inline field_proxy& operator += (T v) { return add_oper_with_equal("+", v); }
            inline field_proxy& operator -= (T v) { return add_oper_with_equal("-", v); }
            inline field_proxy& operator *= (T v) { return add_oper_with_equal("*", v); }
            inline field_proxy& operator /= (T v) { return add_oper_with_equal("/", v); }
            inline field_proxy& operator %= (T v) { return add_oper_with_equal("%", v); }

        private:
            inline set_expr add_operator(const char* op, T v)
            {
                auto str_expr = fmt::format("{}.{}{}{}", table_name(), field_name_, op, util::cast_string(v));
                return set_expr{ str_expr };
            }
            inline field_proxy& add_oper_with_equal(const char* op, T v)
            {
                expr_ = fmt::format("{0}.{1}={0}.{1}{2}{3}", table_name(), field_name_, op, util::cast_string(v));
                return *this;
            }
        private:
            T* val_;
            bool is_init_;
        };

        template<>
        struct field_proxy<std::string> : public select_able<std::string>, public set_expr
        {
            field_proxy(const std::string* table_name, std::string* v, std::string name)
                : select_able<std::string>(name, table_name)
                , set_expr("")
                , val_(v), is_init_(false) {}

            field_proxy& operator=(const field_proxy& o)
            {
                *val_ = *o.val_;
                is_init_ = o.is_init_;
                return *this;
            }

            explicit operator bool() const noexcept
            {
                return is_init_;
            }
            bool operator! ()
            {
                return !is_init_;
            }

            std::string name() const { return field_name_; }
            const std::string& table_name() const { return *table_name_; }

            std::string value()
            {
                return *val_;
            }

            void set_value(std::string v)
            {
                is_init_ = true;
                *val_ = v;
            }

        public:
            inline field_proxy& operator = (std::string v)
            {
                set_value(v);
                auto str_expr = fmt::format("{}.{}='{}'", table_name(), field_name_, util::cast_string(v));
                expr_ = str_expr;
                return *this;
            }
            inline field_proxy& operator = (set_expr v)
            {
                auto str_expr = fmt::format("{}.{}='{}'", table_name(), field_name_, v.str());
                expr_ = str_expr;
                return *this;
            }

            /*inline set_expr operator + (std::string v) { return add_operator("+", v); }
            inline set_expr operator - (std::string v) { return add_operator("-", v); }
            inline set_expr operator * (std::string v) { return add_operator("*", v); }
            inline set_expr operator / (std::string v) { return add_operator("/", v); }
            inline set_expr operator % (std::string v) { return add_operator("%", v); }

            inline field_proxy& operator += (std::string v) { return add_oper_with_equal("+", v); }
            inline field_proxy& operator -= (std::string v) { return add_oper_with_equal("-", v); }
            inline field_proxy& operator *= (std::string v) { return add_oper_with_equal("*", v); }
            inline field_proxy& operator /= (std::string v) { return add_oper_with_equal("/", v); }
            inline field_proxy& operator %= (std::string v) { return add_oper_with_equal("%", v); }*/

        private:
            /*inline set_expr add_operator(const char* op, std::string v)
            {
                auto str_expr = fmt::format("{}.{}{}{}", table_name(), field_name_, op, v);
                return set_expr{ str_expr };
            }
            inline field_proxy& add_oper_with_equal(const char* op, std::string v)
            {
                expr_ = fmt::format("{0}.{1}={1}{2}{3}", table_name(), field_name_, op, v);
                return *this;
            }*/
        private:
            std::string* val_;
            bool is_init_;
        };

        template <typename T>
        struct aggregate : public select_able<T>
        {
            aggregate(std::string str_func)
                : select_able<T>(std::move(str_func), nullptr) {}

            aggregate(std::string str_func, const field_proxy<T> &field)
                : select_able<T>(fmt::format("{}({}.{})", str_func, field.table_name(), field.name()), nullptr)
            {}
        };

        struct expr
        {
            template <typename T>
            expr(const select_able<T> &field, std::string op_val)
                : exprs{ { field.field_name_ + op_val, field.table_name_} }
            {}

            template <typename T>
            expr(const select_able<T> &field, std::string op, T v)
            {
                auto str_expr = fmt::format("{}{}{}", field.field_name_, op, util::cast_string(v));
                exprs.emplace_back(str_expr, field.table_name_);
            }

            template <typename T>
            expr(const field_proxy<T> &field1, std::string op, const field_proxy<T> &field2)
                : exprs
            {
                { field1.field_name_, field1.table_name_ },
                { std::move(op), nullptr },
                { field2.field_name_, field2.table_name_ }
            }
            {}

            std::string str(bool with_table_name) const
            {
                fmt::MemoryWriter wr;
                for (const auto& p : exprs)
                {
                    if (with_table_name && p.second != nullptr)
                    {
                        wr.write("{}.", *p.second);
                    }
                    wr.write(p.first);
                }
                return wr.str();
            }

            inline expr operator && (const expr &right) const
            {
                return and_or(right, " and ");
            }
            inline expr operator || (const expr &right) const
            {
                return and_or(right, " or ");
            }

        protected:
            std::list<std::pair<std::string, const std::string*>> exprs;

            expr and_or(const expr &right, std::string log_op) const
            {
                auto ret = *this;
                auto rigthExprs = right.exprs;
                ret.exprs.emplace_front("(", nullptr);
                ret.exprs.emplace_back(std::move(log_op), nullptr);
                ret.exprs.splice(ret.exprs.cend(), std::move(rigthExprs));
                ret.exprs.emplace_back(")", nullptr);
                return ret;
            }
        };

        // field_proxy / aggregate ? Value

        template <typename T>
        inline expr operator == (const select_able<T> &op, T value)
        {
            return expr(op, "=", std::move(value));
        }

        template <typename T>
        inline expr operator != (const select_able<T> &op, T value)
        {
            return expr(op, "!=", std::move(value));
        }

        template <typename T>
        inline expr operator > (const select_able<T> &op, T value)
        {
            return expr(op, ">", std::move(value));
        }

        template <typename T>
        inline expr operator >= (const select_able<T> &op, T value)
        {
            return expr(op, ">=", std::move(value));
        }

        template <typename T>
        inline expr operator < (const select_able<T> &op, T value)
        {
            return expr(op, "<", std::move(value));
        }

        template <typename T>
        inline expr operator <= (const select_able<T> &op, T value)
        {
            return expr(op, "<=", std::move(value));
        }

        // field_proxy ? field_proxy

        template <typename T>
        inline expr operator == (const field_proxy<T> &op1, const field_proxy<T> &op2)
        {
            return expr{ op1 , "=", op2 };
        }

        template <typename T>
        inline expr operator != (const field_proxy<T> &op1, const field_proxy<T> &op2)
        {
            return expr{ op1, "!=", op2 };
        }

        template <typename T>
        inline expr operator > (const field_proxy<T> &op1, const field_proxy<T> &op2)
        {
            return expr{ op1 , ">", op2 };
        }

        template <typename T>
        inline expr operator >= (const field_proxy<T> &op1, const field_proxy<T> &op2)
        {
            return expr{ op1, ">=", op2 };
        }

        template <typename T>
        inline expr operator < (const field_proxy<T> &op1, const field_proxy<T> &op2)
        {
            return expr{ op1 , "<", op2 };
        }

        template <typename T>
        inline expr operator <= (const field_proxy<T> &op1, const field_proxy<T> &op2)
        {
            return expr{ op1, "<=", op2 };
        }

        // Nullable field_proxy == / != nullptr

        /*template <typename T>
        inline expr operator == (const NullableField<T> &op, std::nullptr_t)
        {
            return expr{ op, " is null" };
        }

        template <typename T>
        inline expr operator != (const NullableField<T> &op, std::nullptr_t)
        {
            return expr{ op, " is not null" };
        }*/

        // String field_proxy & / | std::string

        inline expr operator & (const field_proxy<std::string> &field, std::string val)
        {
            return expr(field, " like ", std::move(val));
        }

        inline expr operator | (const field_proxy<std::string> &field, std::string val)
        {
            return expr(field, " not like ", std::move(val));
        }

        // aggregate Function Helpers

        inline auto count()
        {
            return aggregate<size_t> { "count (*)" };
        }

        template <typename T>
        inline auto count(const field_proxy<T> &field)
        {
            return aggregate<T> { "count", field };
        }

        template <typename T>
        inline auto sum(const field_proxy<T> &field)
        {
            return aggregate<T> { "sum", field };
        }

        template <typename T>
        inline auto avg(const field_proxy<T> &field)
        {
            return aggregate<T> { "avg", field };
        }

        template <typename T>
        inline auto max(const field_proxy<T> &field)
        {
            return aggregate<T> { "max", field };
        }

        template <typename T>
        inline auto min(const field_proxy<T> &field)
        {
            return aggregate<T> { "min", field };
        }
    }
}
