#pragma once
#include <spdlog/spdlog.h>
#include <memory>
#include "../meta/meta.hpp"
#include <boost/filesystem.hpp>

namespace cytx
{
    enum class log_level_t
    {
        trace = 0,
        debug = 1,
        info = 2,
        warn = 3,
        err = 4,
        critical = 5,
        off = 6
    };
    REG_ENUM(log_level_t, trace, debug, info, warn, err, critical, off);

    namespace fs = boost::filesystem;

    class my_formater : public spdlog::formatter
    {
        void format(spdlog::details::log_msg& msg) override
        {
            using namespace spdlog;
            auto tm_time = details::os::localtime(log_clock::to_time_t(msg.time));
            auto duration = msg.time.time_since_epoch();
            auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;

            // Faster (albeit uglier) way to format the line (5.6 million lines/sec under 10 threads)
            msg.formatted << '[' << static_cast<unsigned int>(tm_time.tm_year + 1900) << '-'
                << fmt::pad(static_cast<unsigned int>(tm_time.tm_mon + 1), 2, '0') << '-'
                << fmt::pad(static_cast<unsigned int>(tm_time.tm_mday), 2, '0') << ' '
                << fmt::pad(static_cast<unsigned int>(tm_time.tm_hour), 2, '0') << ':'
                << fmt::pad(static_cast<unsigned int>(tm_time.tm_min), 2, '0') << ':'
                << fmt::pad(static_cast<unsigned int>(tm_time.tm_sec), 2, '0') << '.'
                << fmt::pad(static_cast<unsigned int>(millis), 3, '0') << "] ";

            msg.formatted.write("[{:05}] [{}] [{}] ", msg.thread_id, *msg.logger_name, level::to_str(msg.level));
            /*msg.formatted << '[' << *msg.logger_name << "] ";

            msg.formatted << '[' << level::to_str(msg.level) << "] ";*/
            msg.formatted << fmt::StringRef(msg.raw.data(), msg.raw.size());
            //write eol
            msg.formatted.write(details::os::eol, details::os::eol_size);
        }
    };

    using log_ptr_t = std::shared_ptr<spdlog::logger>;

    using file_sink_t = spdlog::sinks::rotating_file_sink_mt;
    using file_sink_ptr_t = std::shared_ptr<file_sink_t>;

    using console_sink_t = spdlog::sinks::wincolor_stdout_sink_mt;
    using console_sink_ptr_t = std::shared_ptr<console_sink_t>;

    class log_sink_manager
    {
    public:
        log_sink_manager()
        {
            console_sink_ = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
        }
    public:
        static console_sink_ptr_t get_console_sink()
        {
            return instance().console_sink_;
        }
        static file_sink_ptr_t get_file_sink(std::string file_name)
        {
            auto it = instance().file_sinks_.find(file_name);
            if (it != instance().file_sinks_.end())
            {
                return it->second;
            }
            else
            {
                fs::path dir = file_name;
                dir = fs::complete(dir);
                fs::path cur_dir = dir.parent_path();
                if (!fs::exists(cur_dir))
                {
                    fs::create_directories(cur_dir);
                }
                auto rotating = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(file_name, "txt", 1024 * 1024 * 50, 1000);
                instance().file_sinks_[file_name] = rotating;
                return rotating;
            }
        }
    private:
        static log_sink_manager& instance()
        {
            static log_sink_manager l;
            return l;
        }

        std::map<std::string, file_sink_ptr_t> file_sinks_;
        console_sink_ptr_t console_sink_;
    };

    class log
    {
    public:
        static log& get()
        {
            static log _log;
            return _log;
        }

        static log_ptr_t get_log(const std::string& log_name)
        {
            return spdlog::get(log_name);
        }

        static log_ptr_t force_get_log(const std::string& log_name)
        {
            auto ptr = get_log(log_name);
            if (!ptr)
            {
                ptr = spdlog::stdout_logger_mt(log_name);
                ptr->set_level(spdlog::level::level_enum::off);
            }
            return ptr;
        }

        static log_ptr_t init_log(const std::string& file_name, log_level_t lvl = log_level_t::debug, const std::string& logger_name = "logger", bool console_log = true)
        {
            log l(false);
            l.init(file_name, lvl, logger_name, console_log);
            return l.get_log();
        }

        log(bool auto_init = true)
        {
            if (auto_init)
            {
                log_ = spdlog::stdout_logger_mt("___auto_init_log");
                log_->set_level(spdlog::level::level_enum::off);
            }
        }

        void init(const std::string& file_name, log_level_t lvl = log_level_t::debug, const std::string& logger_name = "logger", bool console_log = true)
        {
            auto rotating = log_sink_manager::get_file_sink(file_name);
            if (console_log)
            {
                log_ = spdlog::create(logger_name, spdlog::sinks_init_list{ rotating, log_sink_manager::get_console_sink() });
            }
            else
            {
                log_ = spdlog::create(logger_name, spdlog::sinks_init_list{ rotating });
            }
            log_->set_level((spdlog::level::level_enum)lvl);
            log_->flush_on(spdlog::level::level_enum::err);
            log_->set_formatter(std::make_shared<my_formater>());
        }

        void init(log_level_t lvl = log_level_t::debug, const std::string& logger_name = "logger")
        {
            log_ = spdlog::create(logger_name, spdlog::sinks_init_list{ log_sink_manager::get_console_sink() });
            log_->set_level((spdlog::level::level_enum)lvl);
            log_->flush_on(spdlog::level::level_enum::err);
            log_->set_formatter(std::make_shared<my_formater>());
        }

        log_ptr_t get_log()
        {
            return log_;
        }

        void flush()
        {
            log_->flush();
        }

        void set_level(log_level_t lvl)
        {
            if (lvl >= log_level_t::trace && lvl <= log_level_t::off)
            {
                log_->set_level((spdlog::level::level_enum)lvl);
            }
        }

    private:
        log(const log&) = delete;
        log(log&&) = delete;

        log_ptr_t log_;
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