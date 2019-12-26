#pragma once
#include <string>
#include <fmt/format.h>
#include <boost/algorithm/string.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/sha1.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "random.hpp"
#include "cast.hpp"
#include "network/base/date_time.hpp"

namespace cytx
{
    namespace util
    {
        using memory_wirter_t = fmt::MemoryWriter;
        class random_string
        {
        public:
            //get a fixed length string with random content, contain number upper and lower letters
            static std::string get_string(int length)
            {
                return get_string_with_chars(length, ins().all_char_);
            }

            static std::string get_letter_string(int length)
            {
                return get_string_with_chars(length, ins().letter_char_);
            }

            static std::string get_upper_string(int length)
            {
                std::string str = get_letter_string(length);
                boost::to_upper(str);
                return str;
            }

            static std::string get_lower_string(int length)
            {
                std::string str = get_letter_string(length);
                boost::to_lower(str);
                return str;
            }

            static std::string get_number_string(int length)
            {
                return get_string_with_chars(length, ins().number_char_);
            }

            static std::string get_string_with_chars(int length, const std::string& chars)
            {
                memory_wirter_t mr;
                if (!chars.empty())
                {
                    for (int i = 0; i < length; ++i)
                    {
                        int index = random_util::get_int((int)chars.length() - 1);
                        char c = chars[index];
                        mr << c;
                    }
                }
                return mr.str();
            }

            static std::string get_zero_string(int length)
            {
                memory_wirter_t mr;
                for (int i = 0; i < length; ++i)
                {
                    mr << '0';
                }
                return mr.str();
            }

            //Generate a string of fixed length based on the number, which is not long enough to fill in 0
            //not support less than zero number
            static std::string get_fixed_length_string(int64_t number, int fixed_length)
            {
                memory_wirter_t mr;
                std::string num_str = cast_string(number);
                mr << get_zero_string(fixed_length - (int)num_str.length());
                mr << number;
                return mr.str();
            }

            static std::string get_token()
            {
                boost::uuids::uuid id = ins().uuid_gen_();
                std::string uuid_str = boost::uuids::to_string(id);
                std::string str = fmt::format("{}_{}", uuid_str, date_time::now().total_milliseconds());
                boost::uuids::detail::sha1 sha;
                sha.process_bytes(str.data(), str.length());
                uint32_t digest[5];
                sha.get_digest(digest);
                memory_wirter_t mr;
                for (int i = 0; i < 5; ++i)
                {
                    mr.write("{:x}", digest[i]);
                }
                return mr.str();
            }

        private:
            static random_string& ins()
            {
                static random_string r;
                return r;
            }

            const std::string all_char_ = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
            const std::string letter_char_ = "abcdefghijkllmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
            const std::string number_char_ = "0123456789";
            boost::uuids::random_generator uuid_gen_;
        };
    }

    using random_string = util::random_string;
}