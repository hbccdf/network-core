#pragma once
#include "common.h"
#include "../meta/meta.hpp"
#include <boost/system/error_code.hpp>

namespace cytx {
    namespace rpc
    {
        class application_category : public boost::system::error_category
        {
        public:
            application_category()
            {
                init();
            }
        public:
            const char *name() const noexcept override { return "application"; }
            std::string message(int ev) const override
            {
                auto it = messages_.find(ev);
                if (it != messages_.end())
                    return it->second;
                else
                    return fmt::format("code: {}, msg: 未知错误", ev);
            }

        private:
            void init()
            {
                add_msg(error_code::ok, "操作成功");
                add_msg(error_code::unknown, "未知错误");
                add_msg(error_code::fail, "操作失败");
                add_msg(error_code::timeout, "操作超时");
                add_msg(error_code::cancel, "操作被取消");
                add_msg(error_code::connect_fail, "连接失败");
                add_msg(error_code::badconnect, "无效的连接");
                add_msg(error_code::codec_fail, "编码解码失败");
                add_msg(error_code::invalid_header, "无效的消息头");
                add_msg(error_code::be_disconnected, "主动断开连接");
                add_msg(error_code::repeat_connect, "重复连接");
                add_msg(error_code::no_handler, "没有对应的handler");
                add_msg(error_code::remote_error, "远端出现错误");
            }

            void add_msg(error_code ec, std::string msg)
            {
                messages_.emplace((int)ec, fmt::format("name: {}, msg: {}", to_string(ec).value(), msg));
            }

        private:
            std::map<int, std::string> messages_;
        };

        inline boost::system::error_code get_ec(error_code ec)
        {
            static application_category ac;
            return boost::system::error_code((int)ec, ac);
        }

        class rpc_exception;

        class rpc_result
        {
            friend rpc_exception;
        public:
            rpc_result() noexcept
                : app_ec_()
                , sys_ec_()
                , msg_()
            {
            }

            rpc_result(error_code ec)
                : app_ec_(get_ec(ec))
                , sys_ec_()
                , msg_()
            {
            }

            rpc_result(error_code ec, boost::system::error_code err)
                : app_ec_(get_ec(ec))
                , sys_ec_(err)
                , msg_()
            {
            }

            rpc_result(boost::system::error_code ec)
                : app_ec_(get_ec(!ec ? error_code::ok : error_code::fail))
                , sys_ec_(ec)
                , msg_()
            {
            }

            rpc_result(error_code ec, std::string em)
                : app_ec_(get_ec(ec))
                , sys_ec_()
                , msg_(std::move(em))
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
                return app_ec_.value() == 0 && sys_ec_.value() == 0;
            }

            bool is_app_error() const
            {
                return sys_ec_.value() != 0 || app_ec_.value() != 0;
            }

            error_code code() const { return (error_code)app_ec_.value(); }

            int value() const { return sys_ec_.value(); }

            std::string message() const
            {
                if (is_success())
                    return "成功";
                else if (sys_ec_)
                {
                    return fmt::format("{}, {}", app_ec_.message(), sys_ec_.message());
                }
                else if (msg_.empty())
                {
                    return app_ec_.message();
                }
                else
                {
                    return fmt::format("{}, {}", app_ec_.message(), msg_);
                }
            }

            boost::system::error_code app_ec() const noexcept { return app_ec_; }
            boost::system::error_code sys_ec() const noexcept { return sys_ec_; }
            std::string raw_msg() const noexcept { return msg_; }
        protected:
            boost::system::error_code app_ec_;
            boost::system::error_code sys_ec_;
            std::string msg_;
        };

        class rpc_exception : public rpc_result
        {
        public:
            using rpc_result::rpc_result;

            rpc_exception(rpc_result& r)
                : rpc_result(r)
            {}

            rpc_exception& operator= (rpc_result& r)
            {
                this->app_ec_ = r.app_ec_;
                this->sys_ec_ = r.sys_ec_;
                this->msg_ = r.msg_;
                return *this;
            }

            rpc_result result()
            {
                rpc_result r;
                r.app_ec_ = app_ec_;
                r.sys_ec_ = sys_ec_;
                r.msg_ = msg_;
                return r;
            }
        };

        inline bool operator != (const boost::system::error_code& ec, error_code code)
        {
            return ec.value() != (int)code ||
                ec.category() == boost::system::generic_category() ||
                ec.category() == boost::system::system_category();
        }

        inline bool operator == (const boost::system::error_code& ec, error_code code)
        {
            return !(ec != code);
        }

        inline bool operator != (const rpc_result& ec, error_code code)
        {
            return ec.app_ec() != code;
        }

        inline bool operator == (const rpc_result& ec, error_code code)
        {
            return !(ec != code);
        }
    }
}