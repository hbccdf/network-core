#pragma once
#include <boost/optional.hpp>
#include <vector>
#include <map>
#include <memory>
#include <cstdint>
#include "network/traits/traits.hpp"
#include "network/base/cast.hpp"

namespace cytx {
    namespace orm
    {
        template<typename T>
        auto cast_string(const T& t) -> std::enable_if_t<std::is_same<T, date_time>::value, std::string>
        {
            return fmt::format("'{}'", t.to_string());
        }

        template<typename T>
        auto cast_string(const T& t) -> std::enable_if_t<cytx::is_nullable<T>::value, std::string>
        {
            if (t)
                return cast_string(t.get());
            else
                return "null";
        }

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
            const char* table_name_;

            select_able(std::string field_name, const char* table_name)
                : field_name_(std::move(field_name)), table_name_(table_name)
            {}
        };

        template<typename P, typename T>
        struct field_proxy : public select_able<T>, public set_expr
        {
            using parent_t = P;
            using value_type = typename parent_t::value_type;
            using val_t = T;
            using val_ptr = T value_type::*;

            field_proxy(value_type* p, val_ptr v, const char* name)
                : select_able<T>(name, parent_t::meta_name())
                , set_expr("")
                , p_(p)
                , val_(v), is_init_(false) {}

            field_proxy& operator=(const field_proxy& o)
            {
                p_->*val_ = o.p_->*val_;
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

            const char* name() const { return this->field_name_.c_str(); }
            const char* table_name() const { return this->table_name_; }

            val_t value()
            {
                return p_->*val_;
            }

            void set_value(val_t v)
            {
                is_init_ = true;
                p_->*val_ = v;
            }

        public:
            inline field_proxy& operator = (val_t v)
            {
                set_value(v);
                auto str_expr = fmt::format("{}.{}={}", table_name(), name(), orm::cast_string(v));
                expr_ = str_expr;
                return *this;
            }
            inline field_proxy& operator = (set_expr v)
            {
                auto str_expr = fmt::format("{}.{}={}", table_name(), name(), v.str());
                expr_ = str_expr;
                return *this;
            }

            inline set_expr operator + (val_t v) { return add_operator("+", v); }
            inline set_expr operator - (val_t v) { return add_operator("-", v); }
            inline set_expr operator * (val_t v) { return add_operator("*", v); }
            inline set_expr operator / (val_t v) { return add_operator("/", v); }
            inline set_expr operator % (val_t v) { return add_operator("%", v); }

            inline field_proxy& operator += (val_t v) { return add_oper_with_equal("+", v); }
            inline field_proxy& operator -= (val_t v) { return add_oper_with_equal("-", v); }
            inline field_proxy& operator *= (val_t v) { return add_oper_with_equal("*", v); }
            inline field_proxy& operator /= (val_t v) { return add_oper_with_equal("/", v); }
            inline field_proxy& operator %= (val_t v) { return add_oper_with_equal("%", v); }

        private:
            inline set_expr add_operator(const char* op, val_t v)
            {
                auto str_expr = fmt::format("{}.{}{}{}", table_name(), name(), op, orm::cast_string(v));
                return set_expr{ str_expr };
            }
            inline field_proxy& add_oper_with_equal(const char* op, val_t v)
            {
                expr_ = fmt::format("{0}.{1}={0}.{1}{2}{3}", table_name(), name(), op, orm::cast_string(v));
                return *this;
            }
        private:
            value_type* p_;
            val_ptr val_;
            bool is_init_;
        };

        template<typename P>
        struct field_proxy<P, std::string> : public select_able<std::string>, public set_expr
        {
            using parent_t = P;
            using value_type = typename parent_t::value_type;
            using val_t = std::string;
            using val_ptr = std::string value_type::*;

            field_proxy(value_type* p, val_ptr v, const char* name)
                : select_able(name, parent_t::meta_name())
                , set_expr("")
                , p_(p)
                , val_(v), is_init_(false) {}

            field_proxy& operator=(const field_proxy& o)
            {
                p_->*val_ = o.p_->*val_;
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

            const char* name() const { return field_name_.c_str(); }
            const char* table_name() const { return table_name_; }

            val_t value()
            {
                return p_->*val_;
            }

            void set_value(val_t v)
            {
                is_init_ = true;
                p_->*val_ = v;
            }

        public:
            inline field_proxy& operator = (val_t v)
            {
                set_value(v);
                auto str_expr = fmt::format("{}.{}='{}'", table_name(), name(), v);
                expr_ = str_expr;
                return *this;
            }

        private:
            value_type* p_;
            val_ptr val_;
            bool is_init_;
        };

        template <typename T>
        struct aggregate : public select_able<T>
        {
            aggregate(std::string str_func)
                : select_able<T>(std::move(str_func), nullptr) {}

            template<typename P>
            aggregate(std::string str_func, const field_proxy<P, T>& field)
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
                auto str_expr = fmt::format("{}{}{}", field.field_name_, op, orm::cast_string(v));
                exprs.emplace_back(str_expr, field.table_name_);
            }

            template<typename P, typename T>
            expr(const field_proxy<P, T> &field1, std::string op, const field_proxy<P, T> &field2)
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
                        wr.write("{}.", p.second);
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
            std::list<std::pair<std::string, const char*>> exprs;

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

        template <typename P, typename T>
        inline expr operator == (const field_proxy<P, T> &op1, const field_proxy<P, T> &op2)
        {
            return expr{ op1 , "=", op2 };
        }

        template <typename P, typename T>
        inline expr operator != (const field_proxy<P, T> &op1, const field_proxy<P, T> &op2)
        {
            return expr{ op1, "!=", op2 };
        }

        template <typename P, typename T>
        inline expr operator > (const field_proxy<P, T> &op1, const field_proxy<P, T> &op2)
        {
            return expr{ op1 , ">", op2 };
        }

        template <typename P, typename T>
        inline expr operator >= (const field_proxy<P, T> &op1, const field_proxy<P, T> &op2)
        {
            return expr{ op1, ">=", op2 };
        }

        template <typename P, typename T>
        inline expr operator < (const field_proxy<P, T> &op1, const field_proxy<P, T> &op2)
        {
            return expr{ op1 , "<", op2 };
        }

        template <typename P, typename T>
        inline expr operator <= (const field_proxy<P, T> &op1, const field_proxy<P, T> &op2)
        {
            return expr{ op1, "<=", op2 };
        }

        // Nullable field_proxy == / != nullptr

        template <typename T>
        inline expr operator == (const nullable<T> &op, std::nullptr_t)
        {
            return expr{ op, " is null" };
        }

        template <typename T>
        inline expr operator != (const nullable<T> &op, std::nullptr_t)
        {
            return expr{ op, " is not null" };
        }

        // String field_proxy & / | std::string

        template<typename P>
        inline expr operator & (const field_proxy<P, std::string> &field, std::string val)
        {
            return expr(field, " like ", std::move(val));
        }

        template<typename P>
        inline expr operator | (const field_proxy<P, std::string> &field, std::string val)
        {
            return expr(field, " not like ", std::move(val));
        }

        // aggregate Function Helpers

        inline auto count()
        {
            return aggregate<size_t> { "count (*)" };
        }

        template <typename P, typename T>
        inline auto count(const field_proxy<P, T> &field)
        {
            return aggregate<T> { "count", field };
        }

        template <typename P, typename T>
        inline auto sum(const field_proxy<P, T> &field)
        {
            return aggregate<T> { "sum", field };
        }

        template <typename P, typename T>
        inline auto avg(const field_proxy<P, T> &field)
        {
            return aggregate<T> { "avg", field };
        }

        template <typename P, typename T>
        inline auto max(const field_proxy<P, T> &field)
        {
            return aggregate<T> { "max", field };
        }

        template <typename P, typename T>
        inline auto min(const field_proxy<P, T> &field)
        {
            return aggregate<T> { "min", field };
        }
    }
}
