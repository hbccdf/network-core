#pragma once
#include "common.h"
#include "../serialize/common.hpp"
namespace cytx {
    namespace rpc
    {
        class exception
        {
        public:
            META(error_code_, error_message_);

        public:
            exception() noexcept
                : error_code_(static_cast<int16_t>(error_code::ok))
                , error_message_()
            {
            }

            exception(error_code ec, std::string em)
                : error_code_(static_cast<int16_t>(ec))
                , error_message_(std::move(em))
            {

            }

            error_code get_error_code() const noexcept
            {
                return static_cast<error_code>(error_code_);
            }

            std::string const& get_error_message() const noexcept
            {
                return error_message_;
            }

            void set_message(std::string message)
            {
                error_message_ = std::move(message);
            }

            void set_code(error_code ec) noexcept
            {
                error_code_ = static_cast<int16_t>(ec);
            }

            operator bool() const noexcept
            {
                return error_code_ != static_cast<int16_t>(error_code::ok);
            }

        private:
            int16_t error_code_;
            std::string error_message_;
        };
    }
}