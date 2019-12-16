#pragma once
#include <stdexcept>
#include <exception>
#include <string>
#include <sstream>
#include <fmt/format.h>
#ifdef WIN32
#include <dbghelp.h>
#include <boost/filesystem.hpp>
#endif
#include <type_traits>
#include "date_time.hpp"

namespace cytx
{
    namespace detail
    {
        class ensure_exception_setting
        {
        public:
            static ensure_exception_setting& ins()
            {
                static ensure_exception_setting setting;
                return setting;
            }

        public:
            bool only_ensure_info() const
            {
                return only_ensure_info_;
            }

            void only_ensure_info(bool is_only_ensure_info)
            {
                only_ensure_info_ = is_only_ensure_info;
            }

            std::string dump_dir() const
            {
                return dump_dir_;
            }

            void dump_dir(const std::string& dir)
            {
                dump_dir_ = dir;
            }

            std::string module_name() const
            {
                return module_name_;
            }

            void module_name(const std::string& name)
            {
                module_name_ = name;
            }

        private:
            bool only_ensure_info_ = true;
            std::string dump_dir_ = "log/more_info";
            std::string module_name_;
        };

        using namespace std;
        class ensure_exception : public std::exception
        {
        public:
            ensure_exception(const char* expr, const char* file, const char* func, int line)
                : SMART_ENSURE_A(*this)
                , SMART_ENSURE_B(*this)
            {
                ostringstream ss;
                ss << "ensure falid : " << expr << ", " << endl;
                ss << file << '(' << line << ')' << ":" << func << ", " << endl;
                ss << "context variables : " << endl;
                str_what_ = ss.str();
            }

            ensure_exception(const ensure_exception& other)
                : SMART_ENSURE_A(*this)
                , SMART_ENSURE_B(*this)
                , str_what_(other.str_what_)
                , dump_path_(other.dump_path_)
            {

            }

            template<typename T>
            ensure_exception& add_pair(const char* key, const T& value)
            {
                fmt::MemoryWriter mr;
                mr.write("\t{} : {}\n", key, value);
                str_what_ += mr.str();
                return *this;
            }

            const char* what() const { return str_what_.c_str(); }

            string dump_path() const { return dump_path_; }

            ensure_exception & SMART_ENSURE_A;
            ensure_exception & SMART_ENSURE_B;

#ifdef WIN32
        public:
            ensure_exception & make_dump()
            {
                if (!ensure_exception_setting::ins().only_ensure_info())
                {
                    __try
                    {
                        RaiseException(EXCEPTION_BREAKPOINT, 0, 0, 0);
                    }
                    __except (create_dump(GetExceptionInformation()))
                    {
                    }
                }
                return *this;
            }

        private:
            int create_dump(struct _EXCEPTION_POINTERS *ep)
            {
                unsigned code = ep ? ep->ExceptionRecord->ExceptionCode : 0;

                auto t = date_time::now();
                auto name = ensure_exception_setting::ins().module_name();
                string file_name = fmt::format("{}_more_info_{:04d}{:02d}{:02d}_{:02d}{:02d}{:02d}.log", name, t.year(), t.month(), t.day(), t.hour(), t.minute(), t.second());

                auto dir = ensure_exception_setting::ins().dump_dir();
                wstring path = fmt::format(_T("{}/{}"), dir, file_name);

                if (boost::filesystem::exists(path))
                {
                    return EXCEPTION_EXECUTE_HANDLER;
                }

                HANDLE hfile = CreateFile(path.c_str()
                    , GENERIC_READ | GENERIC_WRITE
                    , FILE_SHARE_READ | FILE_SHARE_WRITE
                    , NULL
                    , OPEN_ALWAYS
                    , FILE_ATTRIBUTE_NORMAL
                    , NULL);

                if (hfile == INVALID_HANDLE_VALUE)
                {
                    return EXCEPTION_EXECUTE_HANDLER;
                }

                MINIDUMP_EXCEPTION_INFORMATION ex = { 0 };
                ex.ExceptionPointers = ep;
                ex.ClientPointers = TRUE;
                ex.ThreadId = GetCurrentThreadId();

                int flag = MiniDumpNormal |
                    MiniDumpWithDataSegs |
                    MiniDumpWithHandleData |
                    MiniDumpWithIndirectlyReferencedMemory |
                    MiniDumpWithPrivateReadWriteMemory |
                    MiniDumpWithThreadInfo |
                    MiniDumpWithUnloadedModules;

                if (::MiniDumpWriteDump(
                    ::GetCurrentProcess()
                    , GetCurrentProcessId()
                    , hfile
                    , (MINIDUMP_TYPE)flag
                    , ep ? &ex : 0
                    , 0
                    , 0))
                {
                }
                else
                {
                }

                CloseHandle(hfile);
                dump_path_ = file_name;
                return EXCEPTION_EXECUTE_HANDLER;
            }
#else
        public:
            ensure_exception & make_dump() { return *this; }
#endif

       public:
           static void only_ensure_info(bool is_only_ensure_info)
           {
               ensure_exception_setting::ins().only_ensure_info(is_only_ensure_info);
           }

           static void dump_dir(const std::string& dir)
           {
               ensure_exception_setting::ins().dump_dir(dir);
           }

           static void init(const std::string& set_module_name, bool is_only_ensure_log = true)
           {
               ensure_exception_setting::ins().module_name(set_module_name);
               ensure_exception_setting::ins().only_ensure_info(is_only_ensure_log);

               if (!is_only_ensure_log)
               {
                   auto dir = ensure_exception_setting::ins().dump_dir();
                   if (!boost::filesystem::exists(dir))
                   {
                       boost::filesystem::create_directories(dir);
                   }
               }
           }

        private:
            string str_what_;
            string dump_path_;
        };
    }

    using ensure_exception = detail::ensure_exception;

    inline ensure_exception make_ensure_exception(const char* expr, const char* file, const char* func, int line)
    {
        return ensure_exception(expr, file, func, line);
    }
}

#define SMART_ENSURE_A(x) SMART_ENSURE_OP(x, B)
#define SMART_ENSURE_B(x) SMART_ENSURE_OP(x, A)
#define SMART_ENSURE_OP(x, next) add_pair(#x, (x)).SMART_ENSURE_##next



#ifndef ENSURE_EXCEPTION_WITHOUT_DUMP
#  if !defined TEST_NOENSURE
#       define ENSURE(expr) {static_assert(std::is_same<decltype(expr), bool>::value, "ENSURE(expr) can only be used with bool");} \
        if((expr)) ; else throw cytx::make_ensure_exception(#expr, __FILE__, __FUNCTION__, __LINE__).make_dump().SMART_ENSURE_A
#  else
#       define ENSURE(expr) {static_assert(std::is_same<decltype(expr), bool>::value, "ENSURE(expr) can only be used with bool");} \
        if((expr)) ; else  cytx::make_ensure_exception(#expr, __FILE__, __FUNCTION__, __LINE__).make_dump().SMART_ENSURE_A
#  endif
#else
#define ENSURE(expr) {static_assert(std::is_same<decltype(expr), bool>::value, "ENSURE(expr) can only be used with bool");} \
  if((expr)) ; else throw cytx::make_ensure_exception(#expr, __FILE__, __FUNCTION__, __LINE__).SMART_ENSURE_A
#endif