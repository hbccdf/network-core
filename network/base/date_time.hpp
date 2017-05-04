#pragma once
#include <string>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>
#include <time.h>
#include <fmt/format.h>
#include <sstream>
#include <locale>

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
            date_time(time_t seconds)
                : time_(from_time_t(seconds))
            {
                time_ += hours(8);
            }
            date_time(int year, int month, int day)
                : time_(date(year, month, day))
            {
            }
            date_time(int year, int month, int day, int hour, int minute, int second)
                : time_(date(year, month, day), time_duration(hour, minute, second))
            {
            }

            date_time(time_t seconds, time_t ms)
                : time_(from_time_t(seconds) + millisec(ms))
            {
                time_ += hours(8);
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
                return date_time(microsec_clock::local_time());
            }

        public:
            date_time add_ms(int ms)
            {
                return time_ + millisec(ms);
            }

            date_time add_seconds(int second)
            {
                time_iterator t_iter(time_, seconds(second));
                ++t_iter;
                return date_time(*t_iter);
            }

            date_time add_minutes(int minute)
            {
                time_iterator t_iter(time_, minutes(minute));
                ++t_iter;
                return date_time(*t_iter);
            }

            date_time add_hours(int hour)
            {
                time_iterator t_iter(time_, hours(hour));
                ++t_iter;
                return date_time(*t_iter);
            }

            date_time add_days(int day)
            {
                return time_ + days(day);
            }

            date_time add_weeks(const int week)
            {
                return time_ + weeks(week);
            }

            date_time add_months(const int month)
            {
                return time_ + months(month);
            }

            date_time add_years(const int year)
            {
                return time_ + years(year);
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

            time_t get_time_t() const
            {
                tm t = get_tm();
                return mktime(&t);
            }

            tm get_tm() const
            {
                return to_tm(time_);
            }

            //format = "%Y-%m-%d %H:%M:%S" %Y=年 %m=月 %d=日 %H=时 %M=分 %S=秒
            std::string to_string(std::string format) const
            {
                boost::gregorian::date_facet df(format.c_str());
                std::stringstream is;
                is.imbue(std::locale(is.getloc(), &df));
                is << time_ << std::endl;
                return is.str();
            }
            //将当前 DateTime 对象的值转换为其等效的短日期字符串表示形式。
            std::string to_short_string() const
            {
                return to_string("%Y-%m-%d");
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
                return time_.time_of_day().hours();
            }
            int minute() const
            {
                return time_.time_of_day().minutes();
            }
            int second() const
            {
                return time_.time_of_day().seconds();
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
                return duration_now().total_seconds();
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
            bool operator == (const date_time &datetime)
            {
                return time_ == datetime.time_;
            }
            bool operator > (const date_time &datetime)
            {
                return time_ > datetime.time_;
            }
            bool operator < (const date_time &datetime)
            {
                return time_ < datetime.time_;
            }
            bool operator >= (const date_time &datetime)
            {
                return time_ >= datetime.time_;
            }
            bool operator <= (const date_time &datetime)
            {
                return time_ <= datetime.time_;
            }
            bool operator != (const date_time &datetime)
            {
                return time_ != datetime.time_;
            }

        private:
            time_duration duration_now() const
            {
                return time_ - ptime(date(1970, 1, 1), time_duration(8, 0, 0));
            }

        private:
            ptime time_;
        };
    }

    using date_time = detail::date_time;
}
