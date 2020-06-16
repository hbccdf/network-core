#pragma once
#include <string>
#include <vector>
#include <map>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/optional.hpp>
#include <fmt/format.h>

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
            using extend_func_t = std::function<bool(const std::string&, std::string&)>;
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

            static std::string join(char, std::string p)
            {
                return p;
            }

            template <typename... Args>
            static std::string join(char j, const std::string& p1, const std::string& p2, Args&&... args)
            {
                if ((!p1.empty() && p1[p1.size() - 1] == j) || (!p2.empty() && p2[0] == j))
                {
                    return join(j, p1 + p2, std::forward<Args>(args)...);
                }

                return join(j, p1 + j + p2, std::forward<Args>(args)...);
            }

            static std::string join(char j, std::vector<std::string>& string_list)
            {
                fmt::MemoryWriter mr;
                for (int i = 0; i < string_list.size(); ++i)
                {
                    mr << string_list[i];

                    if (i + 1 < string_list.size())
                        mr << j;
                }

                return mr.str();
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

            static std::string trim_last_copy(const std::string& str, const std::string& last_str)
            {
                std::string new_str = str;
                trim_last(new_str, last_str);
                return new_str;
            }

            static void trim_last(std::string& str, const std::string& last_str)
            {
                auto range = boost::find_last(str, last_str);
                if (!range.empty())
                {
                    str.erase(range.begin(), str.end());
                }
            }

            static void trim_first(std::string& str, const std::string& first_str)
            {
                auto range = boost::find_first(str, first_str);
                if (!range.empty())
                {
                    str.erase(range.begin(), str.end());
                }
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

            static bool extend_all(std::string& str, const std::string& begin_extend, const std::string& end_extend, extend_func_t extend_func)
            {
                int count = 0;
                size_t offset = 0;
                bool result = false;
                do
                {
                    result = internal_extend(str, offset, begin_extend, end_extend, extend_func);
                    if (result && offset != std::string::npos)
                        ++count;
                    else if (count > 0 && (!result && offset == std::string::npos))
                        result = true;

                } while (result && offset != std::string::npos);

                return count > 0 && result;
            }

            static bool extend_any(std::string& str, const std::string& begin_extend, const std::string& end_extend, extend_func_t extend_func)
            {
                int count = 0;
                size_t offset = 0;
                bool result = false;
                do
                {
                    result = internal_extend(str, offset, begin_extend, end_extend, extend_func);
                    if (result && offset != std::string::npos)
                        ++count;

                } while (offset != std::string::npos);

                return count > 0;
            }

            static bool extend(std::string& str, const std::string& begin_extend, const std::string& end_extend, extend_func_t extend_func)
            {
                size_t offset = 0;
                return internal_extend(str, offset, begin_extend, end_extend, extend_func);
            }

            //��    �ܣ���ͨ������ַ���ƥ��
            //��    ����source��һ����ͨ�ַ�����
            //        match_str��һ���԰���ͨ������ַ�����
            //        matchcaseΪ0�������ִ�Сд���������ִ�Сд��
            //��  ��  ֵ��ƥ�䣬����1�����򷵻�0��
            //ͨ������壺
            //    ��*��  ���������ַ������������ַ�����
            //    ��?��  ��������һ���ַ�������Ϊ�գ�
            static bool match(const std::string& source, const std::string& match_str, bool matchcase = false)
            {
                return matching_string(source.c_str(), match_str.c_str(), matchcase);
            }

            //��  �ܣ�����ƥ�䣬��ͬƥ���ַ���֮���� split_char ����,Ĭ��Ϊ�ո�
            //      �磺��*.h *.cpp��������ƥ�䡰*.h���͡�*.cpp��
            //��  ����match_logic = 0, ��ͬƥ�����else���룻matchcase, �Ƿ��С����
            //����ֵ�����ret_reversed = 0, ƥ�䷵��true������ƥ�䷵��true
            static bool multi_match(const std::string& source, const std::string& match_str, int match_logic = 0, bool matchcase = false, char split_char = ' ', bool ret_reversed = false)
            {
                return multi_matching(source.c_str(), match_str.c_str(), match_logic, ret_reversed, matchcase, split_char);
            }

        private:
            static bool internal_extend(std::string& str, size_t& offset, const std::string& begin_extend, const std::string& end_extend, extend_func_t& extend_func)
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

                offset = pos + str_prop.length();

                return true;
            }

            //��  �ܣ���source�в����ַ���find��find�п��԰���ͨ���ַ���?��
            //��  ����startΪ��source�е���ʼ����λ��
            //����ֵ���ɹ�����ƥ��λ�ã����򷵻�-1
            //ע  �⣺Called by ��bool matching_string()��
            static int finding_string(const char *source, const char *find, int start = 0)
            {
                if (source == NULL || find == NULL || start < 0)
                    return -1;

                int m = static_cast<int>(strlen(source));
                int n = static_cast<int>(strlen(find));

                if (start + n > m)
                    return -1;

                if (n == 0)
                    return start;

                //KMP�㷨
                int* next = new int[n];
                //�õ������ַ�����next����
                {  n--;

                int j, k;
                j = 0;
                k = -1;
                next[0] = -1;

                while (j < n)
                {
                    if (k == -1 || find[k] == '?' || find[j] == find[k])
                    {
                        j++;
                        k++;
                        next[j] = k;
                    }
                    else
                        k = next[k];
                }

                n++;
                }

                int i = start, j = 0;
                while (i < m && j < n)
                {
                    if (j == -1 || find[j] == '?' || source[i] == find[j])
                    {
                        i++;
                        j++;
                    }
                    else
                        j = next[j];
                }

                delete[]next;

                if (j >= n)
                    return i - n;
                else
                    return -1;
            }

            //��    �ܣ���ͨ������ַ���ƥ��
            //��    ����source��һ����ͨ�ַ�����
            //        match��һ���԰���ͨ������ַ�����
            //        matchcaseΪ0�������ִ�Сд���������ִ�Сд��
            //��  ��  ֵ��ƥ�䣬����1�����򷵻�0��
            //ͨ������壺
            //    ��*��  ���������ַ������������ַ�����
            //    ��?��  ��������һ���ַ�������Ϊ�գ�
            static bool matching_string(const char *source, const char *match, bool matchcase = false)
            {
                if (source == NULL || match == NULL)
                    return false;

                if (match[0] == 0)//Is a empty string
                {
                    if (source[0] == 0)
                        return true;
                    else
                        return false;
                }

                int i = 0, j = 0;

                //���ɱȽ�����ʱԴ�ַ���'tmp_src'
                char* tmp_src =
                    new char[(j = static_cast<int>(strlen(source)) + 1)];

                if (matchcase)
                {  //memcpy(tmp_src, source, j);
                    while (*(tmp_src + i) = *(source + i++));
                }
                else
                {  //Lowercase 'source' to 'tmp_src'
                    i = 0;
                    while (source[i])
                    {
                        if (source[i] >= 'A' && source[i] <= 'Z')
                            tmp_src[i] = source[i] - 'A' + 'a';
                        else
                            tmp_src[i] = source[i];

                        i++;
                    }
                    tmp_src[i] = 0;
                }

                //���ɱȽ�����ʱƥ���ַ���'tmp_mat'
                char* tmp_mat = new char[strlen(match) + 1];

                //��match���������ġ�*������һ����*�����Ƶ�tmp_mat��
                i = j = 0;
                while (match[i])
                {
                    tmp_mat[j++] = (!matchcase) ?
                        ((match[i] >= 'A' && match[i] <= 'Z') ?//Lowercase match[i] to tmp_mat[j]
                            match[i] - 'A' + 'a' :
                            match[i]
                            ) :
                        match[i];     //Copy match[i] to tmp_mat[j]
                                      //Merge '*'
                    if (match[i] == '*')
                        while (match[++i] == '*');
                    else
                        i++;
                }
                tmp_mat[j] = 0;

                //��ʼ����ƥ����

                int match_offset, source_offset;

                bool is_matched = true;
                match_offset = source_offset = 0;
                while (tmp_mat[match_offset])
                {
                    if (tmp_mat[match_offset] == '*')
                    {
                        if (tmp_mat[match_offset + 1] == 0)
                        {  //tmp_mat[match_offset]�����һ���ַ�

                            is_matched = true;
                            break;
                        }
                        else
                        {  //tmp_mat[match_offset+1]ֻ����'?'����ͨ�ַ�

                            int sub_offset = match_offset + 1;

                            while (tmp_mat[sub_offset])
                            {
                                if (tmp_mat[sub_offset] == '*')
                                    break;
                                sub_offset++;
                            }

                            if (strlen(tmp_src + source_offset) <
                                size_t(sub_offset - match_offset - 1))
                            {  //Դ�ַ���ʣ�µĳ���С��ƥ�䴮ʣ��Ҫ�󳤶�
                                is_matched = false; //�ж���ƥ��
                                break;      //�˳�
                            }

                            if (!tmp_mat[sub_offset])//sub_offset is point to ender of 'tmp_mat'
                            {  //���ʣ�²����ַ��Ƿ�һһƥ��

                                sub_offset--;
                                int tmp_sub_offset = static_cast<int>(strlen(tmp_src)) - 1;
                                //�Ӻ���ǰ����ƥ��
                                while (tmp_mat[sub_offset] != '*')
                                {
                                    if (tmp_mat[sub_offset] == '?')
                                        ;
                                    else
                                    {
                                        if (tmp_mat[sub_offset] != tmp_src[tmp_sub_offset])
                                        {
                                            is_matched = false;
                                            break;
                                        }
                                    }
                                    sub_offset--;
                                    tmp_sub_offset--;
                                }
                                break;
                            }
                            else//tmp_mat[sub_offset] == '*'
                            {
                                sub_offset -= match_offset;

                                char* szTempFinder = new char[sub_offset];
                                sub_offset--;
                                memcpy(szTempFinder, tmp_mat + match_offset + 1, sub_offset);
                                szTempFinder[sub_offset] = 0;

                                int nPos = finding_string(tmp_src + source_offset, szTempFinder, 0);
                                delete[]szTempFinder;

                                if (nPos != -1)//��'tmp_src+source_offset'���ҵ�szTempFinder
                                {
                                    match_offset += sub_offset;
                                    source_offset += (nPos + sub_offset - 1);
                                }
                                else
                                {
                                    is_matched = false;
                                    break;
                                }
                            }
                        }
                    }    //end of "if(tmp_mat[match_offset] == '*')"
                    else if (tmp_mat[match_offset] == '?')
                    {
                        if (!tmp_src[source_offset])
                        {
                            is_matched = false;
                            break;
                        }
                        if (!tmp_mat[match_offset + 1] && tmp_src[source_offset + 1])
                        {  //���tmp_mat[match_offset]�����һ���ַ���
                           //��tmp_src[source_offset]�������һ���ַ�
                            is_matched = false;
                            break;
                        }
                        match_offset++;
                        source_offset++;
                    }
                    else//tmp_mat[match_offset]Ϊ�����ַ�
                    {
                        if (tmp_src[source_offset] != tmp_mat[match_offset])
                        {
                            is_matched = false;
                            break;
                        }
                        if (!tmp_mat[match_offset + 1] && tmp_src[source_offset + 1])
                        {
                            is_matched = false;
                            break;
                        }
                        match_offset++;
                        source_offset++;
                    }
                }

                delete[]tmp_src;
                delete[]tmp_mat;
                return is_matched;
            }

            //��  �ܣ�����ƥ�䣬��ͬƥ���ַ���֮���� split_char ����,Ĭ��Ϊ�ո�
            //      �磺��*.h *.cpp��������ƥ�䡰*.h���͡�*.cpp��
            //��  ����match_logic = 0, ��ͬƥ�����else���룻matchcase, �Ƿ��С����
            //����ֵ�����ret_reversed = 0, ƥ�䷵��true������ƥ�䷵��true
            static bool multi_matching(const char *source, const char *match, int match_logic = 0, bool ret_reversed = false, bool matchcase = false, char split_char = ' ')
            {
                if (source == NULL || match == NULL)
                    return false;

                char* sub_match = new char[strlen(match) + 1];
                bool is_match;

                if (match_logic == 0)//���
                {
                    is_match = 0;
                    int i = 0;
                    int j = 0;
                    while (1)
                    {
                        if (match[i] != 0 && match[i] != split_char)
                            sub_match[j++] = match[i];
                        else
                        {
                            sub_match[j] = 0;
                            if (j != 0)
                            {
                                is_match = matching_string(source, sub_match, matchcase);
                                if (is_match)
                                    break;
                            }
                            j = 0;
                        }

                        if (match[i] == 0)
                            break;
                        i++;
                    }
                }
                else//����
                {
                    is_match = 1;
                    int i = 0;
                    int j = 0;
                    while (1)
                    {
                        if (match[i] != 0 && match[i] != ',')
                            sub_match[j++] = match[i];
                        else
                        {
                            sub_match[j] = 0;

                            is_match = matching_string(source, sub_match, matchcase);
                            if (!is_match)
                                break;

                            j = 0;
                        }

                        if (match[i] == 0)
                            break;
                        i++;
                    }
                }

                delete[]sub_match;

                if (ret_reversed)
                    return !is_match;
                else
                    return is_match;
            }
        };
    }

    using string_util = util::string_util;
}