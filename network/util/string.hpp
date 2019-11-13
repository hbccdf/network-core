#pragma once
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

namespace cytx
{
    namespace util
    {
        inline bool is_empty_or_white_spate(const std::string& str)
        {
            return str.empty() || boost::trim_copy(str).empty();
        }

        class string_util
        {
        public:
            static void split(std::vector<std::string>& strs, const std::string& input, const std::string& chars)
            {
                boost::split(strs, input, boost::is_any_of(chars), boost::token_compress_on);
            }

            static std::vector<std::string> split(const std::string& input, const std::string& chars)
            {
                std::vector<std::string> strs;
                split(strs, input, chars);
                return strs;
            }

            static std::map<std::string, std::string> split_to_map(const std::string& input)
            {
                std::map<std::string, std::string> result;

                std::vector<std::string> strs = string_util::split(input, ";|");
                for (auto& str : strs)
                {
                    std::vector<std::string> pairs = string_util::split(str, "=");
                    if (pairs.size() != 2)
                        continue;
                    result[pairs[0]] = pairs[1];
                }
                return result;
            }

            static boost::optional<std::pair<std::string, std::string>> split_by_string(std::string str, const std::string& split_field)
            {
                boost::optional<std::pair<std::string, std::string>> result;
                auto pos = str.find(split_field);
                if (pos != std::string::npos)
                {
                    std::pair<std::string, std::string> p;
                    p.first = str.substr(0, pos);
                    p.second = str.substr(pos + std::string(split_field).length());
                    result = p;
                }
                return result;
            }

            static boost::optional<std::pair<std::string, std::string>> split_by_strings(std::string str, const std::vector<std::string>& splits)
            {
                boost::optional<std::pair<std::string, std::string>> result;
                for (auto & s : splits)
                {
                    result = split_by_string(str, s);
                    if (result)
                        break;
                }
                return result;
            }

            static std::vector<std::string> split_of_advanced(const std::string& input, const std::string& seperator = " \t",
                const std::string& quote = "'\"", const std::string& escape = "\\")
            {
                using tokenizer_t = boost::tokenizer<boost::escaped_list_separator<char>, std::string::const_iterator, std::string>;

                std::vector<std::string> strs;

                tokenizer_t tok(input.begin(), input.end(), boost::escaped_list_separator<char>(escape, seperator, quote));
                for (auto it = tok.begin(); it != tok.end(); ++it)
                {
                    if (!it->empty())
                        strs.push_back(*it);
                }
                return strs;
            }

            static std::string trim_copy(const std::string& str)
            {
                return boost::trim_copy(str);
            }

            static void trim(std::string& str)
            {
                boost::trim(str);
            }

            static std::string trim_copy(const std::string& str, const std::string& chars)
            {
                return boost::trim_copy_if(str, boost::is_any_of(chars));
            }

            static void trim(std::string& str, const std::string& chars)
            {
                boost::trim_if(str, boost::is_any_of(chars));
            }

            static bool contains_chars(const std::string& str, const std::string& chars)
            {
                return std::any_of(str.begin(), str.end(), [&chars](auto c) -> bool { return chars.find(c) != std::string::npos; });
            }

            static bool contains(const std::string& str, const std::string& other)
            {
                return str.find(other) != std::string::npos;
            }

            static void replace(std::string& str, const std::string& search_str, const std::string& replace_str)
            {
                boost::replace_all(str, search_str, replace_str);
            }

            static bool extend_all(std::string& str, const std::string& begin_extend, const std::string& end_extend, std::function<bool(const std::string&, std::string&)> extend_func)
            {
                size_t offset = 0;
                bool result = true;
                do
                {
                    result = internal_extend(str, offset, begin_extend, end_extend, extend_func);
                } while (result && offset != std::string::npos);

                return result;
            }

            static bool extend_any(std::string& str, const std::string& begin_extend, const std::string& end_extend, std::function<bool(const std::string&, std::string&)> extend_func)
            {
                size_t offset = 0;
                bool result = false;
                do
                {
                    result = internal_extend(str, offset, begin_extend, end_extend, extend_func);
                } while (offset != std::string::npos);

                return result;
            }

            static bool extend(std::string& str, const std::string& begin_extend, const std::string& end_extend, std::function<bool(const std::string&, std::string&)> extend_func)
            {
                size_t offset = 0;
                return internal_extend(str, offset, begin_extend, end_extend, extend_func);
            }

        private:
            static bool internal_extend(std::string& str, size_t& offset, const std::string& begin_extend, const std::string& end_extend, std::function<bool(const std::string&, std::string&)> extend_func)
            {
                auto pos = str.find(begin_extend, offset);
                offset = std::string::npos;

                if (pos == std::string::npos)
                    return false;

                auto end_pos = str.find(end_extend, pos);
                if (end_pos == std::string::npos)
                    return false;

                size_t begin_extend_length = begin_extend.length();
                size_t end_extend_length = end_extend.length();

                offset = end_pos + end_extend_length;

                std::string str_prop, new_str;
                auto str_val = str.substr(pos + begin_extend_length, end_pos - (pos + begin_extend_length));
                if (!extend_func(str_val, str_prop))
                    return false;

                str.replace(pos, end_pos - pos + end_extend_length, str_prop);

                return true;
            }
        };
    }

    using string_util = util::string_util;
}