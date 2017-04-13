#pragma once
#include <spdlog/spdlog.h>
#include <memory>

namespace cytx
{
    class log
    {
    public:
        static log& get()
        {
            static log _log;
            return _log;
        }

        static auto get_stdout_sink()
        {
            static std::shared_ptr<spdlog::sinks::wincolor_stdout_sink_mt> out_sink_;
            if(out_sink_ == nullptr)
                out_sink_ = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
            return out_sink_;
        }

        log(bool auto_init = true)
        {
            if (auto_init)
            {
                log_ = spdlog::stdout_logger_mt("init");
                log_->set_level(spdlog::level::level_enum::off);
            }
        }

        void init(const std::string& file_name, spdlog::level::level_enum lvl = spdlog::level::level_enum::debug, const std::string& logger_name = "logger")
        {
            auto rotating = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(file_name, "txt", 1024 * 1024 * 50, 3);
            log_ = spdlog::create(logger_name, spdlog::sinks_init_list{ rotating, get_stdout_sink() });
            log_->set_level(lvl);
            log_->flush_on(spdlog::level::level_enum::err);
        }

        void init(spdlog::level::level_enum lvl = spdlog::level::level_enum::debug, const std::string& logger_name = "logger")
        {
            log_ = spdlog::create(logger_name, spdlog::sinks_init_list{ get_stdout_sink() });
            log_->set_level(lvl);
            log_->flush_on(spdlog::level::level_enum::err);
        }

        std::shared_ptr<spdlog::logger> get_log()
        {
            return log_;
        }

        void flush()
        {
            log_->flush();
        }

    private:
        log(const log&) = delete;
        log(log&&) = delete;

        std::shared_ptr<spdlog::logger> log_;
    };


    template<typename... Args>
    static inline void LOG_TRACE(const char* fmt, const Args&... args)
    {
        log::get().get_log()->trace(fmt, args...);
    }

    template<typename... Args>
    static inline void LOG_DEBUG(const char* fmt, const Args&... args)
    {
        log::get().get_log()->debug(fmt, args...);
    }

    template<typename... Args>
    static inline void LOG_INFO(const char* fmt, const Args&... args)
    {
        log::get().get_log()->info(fmt, args...);
    }

    template<typename... Args>
    static inline void LOG_WARN(const char* fmt, const Args&... args)
    {
        log::get().get_log()->warn(fmt, args...);
    }

    template<typename... Args>
    static inline void LOG_ERROR(const char* fmt, const Args&... args)
    {
        log::get().get_log()->error(fmt, args...);
    }

    template<typename... Args>
    static inline void LOG_CRITICAL(const char* fmt, const Args&... args)
    {
        log::get().get_log()->critical(fmt, args...);
    }
}