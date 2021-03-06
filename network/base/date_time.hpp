﻿#pragma once
#include <string>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>
#include <time.h>
#include <fmt/format.h>
#include "network/traits/traits.hpp"

namespace cytx
{
    namespace detail
    {
        using namespace boost::gregorian;
        using namespace boost::posix_time;

        class date_time
        {
            friend std::ostream & operator << (std::ostream & stream, const date_time& t)
            {
                stream << t.to_string();
                return stream;
            }
        public:

            date_time()
                : time_(microsec_clock::local_time())
            {
            }

            date_time(int year, int month, int day)
                : time_(date(year, month, day))
            {
            }
            date_time(int year, int month, int day, int hour, int minute, int second)
                : time_(date(year, month, day), time_duration(hour, minute, second))
            {
            }

            date_time(ptime time)
                : time_(time)
            {
            }

            date_time(std::string str_date_time)
                : time_(time_from_string(str_date_time))
            {
            }

            static date_time parse(std::string str_date_time)
            {
                if (str_date_time == "")
                {
                    return now();
                }
                return std::move(date_time(str_date_time));
            }

            static date_time now()
            {
                return date_time();
            }

            static date_time from_local(time_t second, int ms = 0)
            {
                return date_time(second, ms, false);
            }

            static date_time from_utc(time_t second, int ms = 0)
            {
                return date_time(second, ms, true);
            }

            static date_time from_local_milliseconds(int64_t milliseconds)
            {
                return date_time(milliseconds / 1000, milliseconds % 1000, false);
            }

            static date_time from_utc_milliseconds(int64_t milliseconds)
            {
                return date_time(milliseconds / 1000, milliseconds % 1000, true);
            }

        public:
            date_time add_ms(int ms) const
            {
                return time_ + millisec(ms);
            }

            date_time add_seconds(int second) const
            {
                return time_ + seconds(second);
            }

            date_time add_minutes(int minute) const
            {
                return time_ + minutes(minute);
            }

            date_time add_hours(int hour) const
            {
                return time_ + hours(hour);
            }

            date_time add_days(int day) const
            {
                return time_ + days(day);
            }

            date_time add_weeks(int week) const
            {
                return time_ + weeks(week);
            }

            date_time add_months(int month) const
            {
                return time_ + months(month);
            }

            date_time add_years(int year) const
            {
                return time_ + years(year);
            }

            date_time to_utc() const
            {
                return date_time(time_ - seconds(zone_seconds()));
            }

        public:

            static int compare(const date_time &time1, const date_time &time2)
            {
                return time1.compare_to(time2);
            }

            int compare_to(const date_time & time) const
            {
                if (time_ > time.time_)
                {
                    return 1;
                }
                else if (time_ == time.time_)
                {
                    return 0;
                }
                return -1;
            }

            bool equals(const date_time& time) const
            {
                return time_ == time.time_;
            }

            std::string to_string() const
            {
                std::tm tm_time = get_tm();

                fmt::MemoryWriter ss;
                ss << static_cast<unsigned int>(tm_time.tm_year + 1900) << '-'
                    << fmt::pad(static_cast<unsigned int>(tm_time.tm_mon + 1), 2, '0') << '-'
                    << fmt::pad(static_cast<unsigned int>(tm_time.tm_mday), 2, '0') << ' '
                    << fmt::pad(static_cast<unsigned int>(tm_time.tm_hour), 2, '0') << ':'
                    << fmt::pad(static_cast<unsigned int>(tm_time.tm_min), 2, '0') << ':'
                    << fmt::pad(static_cast<unsigned int>(tm_time.tm_sec), 2, '0');

                return ss.str();
            }

            std::string to_string(const std::string& format_str) const
            {
                const char* str = format_str.c_str();
                fmt::MemoryWriter writer;
                format(writer, str, *this, true);
                return writer.str();
            }

            static void format(fmt::BasicWriter<char> &writer, const char *&format_str, const date_time& t, bool self_to_string = false)
            {
                const char* s = format_str;
                if (*s == ':' || self_to_string)
                {
                    if(*s == ':')
                        ++s;

                    while (*s != '}' && *s != 0)
                    {
                        if (*s == 'm' && *(s + 1) == 's')
                        {
                            writer << fmt::pad((uint32_t)t.ms(), 3, '0');
                            ++s;
                        }
                        else if (*s == 'Y')
                        {
                            writer << fmt::pad((uint32_t)t.year(), 4, '0');
                        }
                        else if (*s == 'y')
                        {
                            int year = t.year() % 100;
                            writer << fmt::pad((uint32_t)year, 4, '0');
                        }
                        else if (*s == 'm')
                        {
                            writer << fmt::pad((uint32_t)t.month(), 2, '0');
                        }
                        else if (*s == 'd' || *s == 'D')
                        {
                            writer << fmt::pad((uint32_t)t.day(), 2, '0');
                        }
                        else if (*s == 'h')
                        {
                            int hour = t.hour();
                            if (hour > 12)
                                hour -= 12;

                            writer << fmt::pad((uint32_t)hour, 2, '0');
                        }
                        else if (*s == 'H')
                        {
                            writer << fmt::pad((uint32_t)t.hour(), 2, '0');
                        }
                        else if (*s == 'M')
                        {
                            writer << fmt::pad((uint32_t)t.minute(), 2, '0');
                        }
                        else if (*s == 's' || *s == 'S')
                        {
                            writer << fmt::pad((uint32_t)t.second(), 2, '0');
                        }
                        else if (*s == 'z' || *s == 'Z')
                        {
                            writer.write("{:+03}", t.zone_hours());
                        }
                        else
                        {
                            writer << *s;
                        }

                        ++s;
                    }

                    if (*s == '}')
                        ++s;

                    format_str = s;
                }
                else
                {
                    writer << t.to_string();
                }
            }

        public:
            int year() const
            {
                return time_.date().year();
            }
            int month() const
            {
                return time_.date().month();
            }
            int day() const
            {
                return time_.date().day();
            }
            int hour() const
            {
                return (int)time_.time_of_day().hours();
            }
            int minute() const
            {
                return (int)time_.time_of_day().minutes();
            }
            int second() const
            {
                return (int)time_.time_of_day().seconds();
            }
            int ms() const
            {
                auto total_milli = time_.time_of_day().total_milliseconds();
                return total_milli % 1000;
            }
            int day_of_week() const
            {
                return time_.date().day_of_week();
            }
            int day_of_year() const
            {
                return time_.date().day_of_year();
            }
            int total_seconds() const
            {
                return (int)duration_now().total_seconds();
            }
            int64_t total_milliseconds() const
            {
                return duration_now().total_milliseconds();
            }
            int64_t total_microseconds() const
            {
                return duration_now().total_microseconds();
            }
            int64_t total_nanoseconds() const
            {
                return duration_now().total_nanoseconds();
            }
            int64_t ticks() const
            {
                return duration_now().ticks();
            }

            int utc_seconds() const
            {
                return total_seconds() - inter_zone_seconds();
            }
            int64_t utc_milliseconds() const
            {
                return total_milliseconds() - inter_zone_seconds() * 1000;
            }
            int64_t utc_nanoseconds() const
            {
                return total_nanoseconds() - inter_zone_seconds() * 1000 * 1000;
            }

            static int zone_seconds()
            {
                return date_time::now().inter_zone_seconds();
            }

            static int zone_hours()
            {
                return zone_seconds() / 3600;
            }

            //返回指定年和月中的天数
            static int day_in_month(int year, int month)
            {
                static const int day_of_month[13] = { 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

                int m = (month < 1 || month > 12) ? 0 : month;

                if (m == 2 && (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0)))
                {
                    return 29;
                }
                else
                {
                    return day_of_month[m];
                }
            }
        public:
            bool operator == (const date_time &datetime) const
            {
                return time_ == datetime.time_;
            }
            bool operator > (const date_time &datetime) const
            {
                return time_ > datetime.time_;
            }
            bool operator < (const date_time &datetime) const
            {
                return time_ < datetime.time_;
            }
            bool operator >= (const date_time &datetime) const
            {
                return time_ >= datetime.time_;
            }
            bool operator <= (const date_time &datetime) const
            {
                return time_ <= datetime.time_;
            }
            bool operator != (const date_time &datetime) const
            {
                return time_ != datetime.time_;
            }

            operator ptime() const
            {
                return time_;
            }

        private:
            date_time(time_t second, int ms, bool is_utc)
            {
                long neg_val = 0;
                if (second < 0)
                {
                    neg_val = -1;
                    second = 0;
                }
                if (is_utc)
                {
                    time_ = ptime_from_tm(*localtime(&second)) + millisec(ms) + seconds(neg_val);
                }
                else
                {
                    time_ = from_time_t(second) + millisec(ms) + seconds(neg_val);
                }
            }

            time_duration duration_now() const
            {
                return time_ - ptime(date(1970, 1, 1));
            }
            int inter_zone_seconds() const
            {
                return total_seconds() - static_cast<int>(get_time_t());
            }
            time_t get_time_t() const
            {
                tm t = get_tm();
                return mktime(&t);
            }

            tm get_tm() const
            {
                return to_tm(time_);
            }

        private:
            ptime time_;
        };
    }

    using date_time = detail::date_time;

    IS_SAME_TYPE(date_time);
}

namespace fmt
{
    inline auto format(fmt::BasicFormatter<char> &f, const char *&format_str, const cytx::date_time& t)
    {
        cytx::date_time::format(f.writer(), format_str, t);
    }
}
