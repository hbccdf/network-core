#pragma once
#include <memory>
#include <spdlog/spdlog.h>
#include <boost/filesystem.hpp>
#include "network/meta/meta.hpp"

namespace spdlog
{
    namespace level
    {
        using log_level_t = level_enum;
        REG_ENUM(log_level_t, trace, debug, info, warn, err, critical, off);
    }
}

namespace cytx
{
    using log_level_t = spdlog::level::level_enum;

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

            if (msg.func_name != nullptr && msg.func_line >= 0)
            {
                msg.formatted.write("[{}:{}] ", msg.func_name, msg.func_line);
            }

            msg.formatted << fmt::StringRef(msg.raw.data(), msg.raw.size());
            //write eol
            msg.formatted.write(details::os::eol, details::os::eol_size);
        }
    };

#ifdef LINUX
    using color_out_t = spdlog::sinks::ansicolor_sink;
#else
    using color_out_t = spdlog::sinks::wincolor_stdout_sink_mt;
#endif

    using log_ptr_t = std::shared_ptr<spdlog::logger>;

    using file_sink_t = spdlog::sinks::rotating_file_sink_mt;
    using file_sink_ptr_t = std::shared_ptr<file_sink_t>;

    using console_sink_t = color_out_t;
    using console_sink_ptr_t = std::shared_ptr<console_sink_t>;

    using log_msg_t = spdlog::details::log_msg;
    using sink_ptr = spdlog::sink_ptr;

    class log_adapter_base;
    class log_sink_manager
    {
    public:
        log_sink_manager()
        {
#ifdef LINUX
            console_sink_ = std::make_shared<color_out_t>(std::make_shared<spdlog::sinks::stdout_sink_mt>());
#else
            console_sink_ = std::make_shared<color_out_t>();
#endif
        }
    public:
        static console_sink_ptr_t get_console_sink()
        {
            return ins().console_sink_;
        }
        static file_sink_ptr_t get_file_sink(std::string file_name)
        {
            auto it = ins().file_sinks_.find(file_name);
            if (it != ins().file_sinks_.end())
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
                ins().file_sinks_[file_name] = rotating;
                return rotating;
            }
        }
        static std::vector<log_adapter_base*>& get_adapters()
        {
            return ins().adapters_;
        }

        static void add_adapter(log_adapter_base* adapter)
        {
            ins().adapters_.push_back(adapter);
        }
    private:
        static log_sink_manager& ins()
        {
            static log_sink_manager l;
            return l;
        }

        std::map<std::string, file_sink_ptr_t> file_sinks_;
        console_sink_ptr_t console_sink_;
        std::vector<log_adapter_base*> adapters_;
    };

    class log_adapter_base : public spdlog::sinks::base_sink<std::mutex>
    {
    public:
        log_adapter_base(bool use_lock)
            : use_lock_(use_lock)
        {}

    protected:
        void log(const log_msg_t& msg) override
        {
            if (use_lock_)
            {
                std::lock_guard<std::mutex> lock(_mutex);
                _sink_it(msg);
            }
            else
            {
                _sink_it(msg);
            }
        }

        void flush() override
        {

        }

    private:
        bool use_lock_;
    };

    class log_adapter : public log_adapter_base
    {
    public:
        using log_func_t = std::function<void(int, const char*)>;

    public:
        log_adapter(log_func_t func, bool show_time, bool use_lock = true)
            : log_adapter_base(use_lock)
            , func_(func)
            , show_time_(show_time)
        {
        }

    protected:
        void _sink_it(const log_msg_t& msg) override
        {
            if (show_time_)
            {
                func_((int)msg.level, msg.formatted.c_str());
            }
            else
            {
                func_((int)msg.level, msg.raw.c_str());
            }
        }

    private:
        log_func_t func_;
        bool show_time_;
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
                ptr->set_level(log_level_t::err);
            }
            return ptr;
        }

        static void add_adapter(log_adapter_base* adapter)
        {
            log_sink_manager::add_adapter(adapter);
        }

        static log_ptr_t init_log(const std::string& file_name, log_level_t lvl = log_level_t::debug, const std::string& logger_name = "logger", bool console_log = true, bool adapter_log = true)
        {
            log l(false);
            l.init(file_name, lvl, logger_name, console_log, adapter_log);
            return l.get_log();
        }

        static log_ptr_t init_log(log_level_t lvl = log_level_t::debug, const std::string& logger_name = "logger", bool console_log = true, bool adapter_log = true)
        {
            log l(false);
            l.init(lvl, logger_name, console_log, adapter_log);
            return l.get_log();
        }

    public:
        log_ptr_t get_log()
        {
            return log_;
        }

        void init(const std::string& file_name, log_level_t lvl = log_level_t::debug, const std::string& logger_name = "logger", bool console_log = true, bool adapter_log = true)
        {
            auto rotating = log_sink_manager::get_file_sink(file_name);

            std::vector<spdlog::sink_ptr> sinks;
            sinks.push_back(rotating);
            inter_init(logger_name, lvl, sinks, console_log, adapter_log);
        }

        void init(log_level_t lvl = log_level_t::debug, const std::string& logger_name = "logger", bool console_log = true, bool adapter_log = true)
        {
            std::vector<spdlog::sink_ptr> sinks;
            inter_init(logger_name, lvl, sinks, console_log, adapter_log);
        }

        void flush()
        {
            log_->flush();
        }

        void set_level(log_level_t lvl)
        {
            if (lvl >= log_level_t::trace && lvl <= log_level_t::off)
            {
                log_->set_level(lvl);
            }
        }

        bool is_init() const
        {
            return false;
        }

    private:
        log(bool auto_init = true)
        {
            if (auto_init)
            {
                log_ = spdlog::stdout_logger_mt("___auto_init_log");
                log_->set_level(log_level_t::off);
                is_init_ = true;
            }
        }

        void inter_init(const std::string& logger_name, log_level_t lvl, std::vector<spdlog::sink_ptr>& sinks, bool console_log, bool adapter_log)
        {
            if (console_log)
            {
                sinks.push_back(log_sink_manager::get_console_sink());
            }
            if (adapter_log)
            {
                add_adapter_sinks(sinks);
            }

            inter_init(logger_name, lvl, sinks);
        }

        void inter_init(const std::string& logger_name, log_level_t lvl, std::vector<spdlog::sink_ptr>& sinks)
        {
            log_ = spdlog::create(logger_name, sinks.begin(), sinks.end());

            log_->set_level(lvl);
            log_->flush_on(log_level_t::err);
            log_->set_formatter(std::make_shared<my_formater>());
            is_init_ = true;
        }

        void add_adapter_sinks(std::vector<spdlog::sink_ptr>& sinks)
        {
            for (auto adapter : log_sink_manager::get_adapters())
            {
                if (adapter)
                {
                    sinks.emplace_back(sink_ptr(adapter, [](auto ptr) {}));
                }
            }
        }

        log(const log&) = delete;
        log(log&&) = delete;

    private:
        log_ptr_t log_;
        bool is_init_ = false;
    };

}

#ifndef NO_GLOBAL_LOG_FUNC

#ifdef LOG_INFO
#undef LOG_INFO
#endif

#ifdef LOG_ERROR
#undef LOG_ERROR
#endif

template<typename... Args>
static inline void LOG_TRACE(const char* fmt, const Args&... args)
{
    cytx::log::get().get_log()->trace(fmt, args...);
}

template<typename... Args>
static inline void LOG_DEBUG(const char* fmt, const Args&... args)
{
    cytx::log::get().get_log()->debug(fmt, args...);
}

template<typename... Args>
static inline void LOG_INFO(const char* fmt, const Args&... args)
{
    cytx::log::get().get_log()->info(fmt, args...);
}

template<typename... Args>
static inline void LOG_WARN(const char* fmt, const Args&... args)
{
    cytx::log::get().get_log()->warn(fmt, args...);
}

template<typename... Args>
static inline void LOG_ERROR(const char* fmt, const Args&... args)
{
    cytx::log::get().get_log()->error(fmt, args...);
}

template<typename... Args>
static inline void LOG_CRITICAL(const char* fmt, const Args&... args)
{
    cytx::log::get().get_log()->critical(fmt, args...);
}

#endif


#define LOGGER_TRACE(fmt, ...) cytx::log::get().get_log()->trace(__LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#define LOGGER_DEBUG(fmt, ...) cytx::log::get().get_log()->debug(__LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#define LOGGER_INFO(fmt, ...) cytx::log::get().get_log()->info(__LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#define LOGGER_WARN(fmt, ...) cytx::log::get().get_log()->warn(__LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#define LOGGER_ERROR(fmt, ...) cytx::log::get().get_log()->error(__LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#define LOGGER_CRITICAL(fmt, ...) cytx::log::get().get_log()->critical(__LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#define LOGGER_LOG(lvl, fmt, ...) cytx::log::get().get_log()->log(___LINE__, __FUNCTION__, fmt, __VA_ARGS__)