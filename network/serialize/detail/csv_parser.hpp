#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <memory>
#include <boost/algorithm/string.hpp>

namespace cytx
{
    namespace parser
    {
        namespace detail
        {
            using namespace std;

            enum class csv_error
            {
                ok,
                no_header,
                no_key,
                index_overflow,
                over_max_size,
                has_empty_line,
                invalid_newline_character,
                syntax_error,
            };

            class csv_exception : public std::exception
            {
            public:
                csv_exception(csv_error err_code, string err_msg) : err_code_(err_code), err_msg_(err_msg) {}

                virtual char const* what() const override { return err_msg_.c_str(); }

                string message() const { return err_msg_; }

                csv_error err_code() const { return err_code_; }
            private:
                csv_error err_code_ = csv_error::ok;
                string err_msg_;
            };

            class csv;
            class csv_row
            {
                friend class csv;
            public:
                csv_row(const vector<string>* header = nullptr) : header_(header) {}
            public:
                size_t size() const { return values_.size(); }
                const string& operator[](size_t index) const
                {
                    if (index < size())
                        return values_[index];

                    throw csv_exception(csv_error::index_overflow, "index overflowed the column size");
                }

                const string& operator[](const string& key) const
                {
                    if (header_ == nullptr)
                        throw csv_exception(csv_error::no_header, "there is no header, can not find the key");

                    auto it = std::find(header_->begin(), header_->end(), key);
                    if (it == header_->end())
                        throw csv_exception(csv_error::no_key, "can not find the key");

                    return values_[it - header_->begin()];
                }
            protected:
                void push_back(const string& value)
                {
                    string trim_val = boost::algorithm::trim_copy_if(value, boost::algorithm::is_any_of(" \t\r\n"));
                    values_.push_back(trim_val);
                }
                void set_header(const vector<string>* header) { header_ = header; }
            protected:
                const vector<string>* header_ = nullptr;
                vector<string> values_;
            };

            // 定义状态
            enum state_type
            {
                new_field_start,                   // 新字段开始
                non_quotes_field,                  // 非引号字段
                quotes_field,                      // 引号字段
                field_separator,                   // 字段分隔
                quote_in_quotes_field,             // 引号字段中的引号
                row_separator,                     // 行分隔符字符1，回车
                error,                             // 语法错误
                newline_in_quote_field,            // 引号中的换行符
            };

            class csv
            {
            public:
                csv(bool first_line_is_header = true, bool header_is_in_rows = false)
                    : first_line_is_header_(first_line_is_header)
                    , header_is_in_rows_(header_is_in_rows) {};

                void parse_string(const string& csv_string)
                {
                    //清空之前的内容
                    header_.clear();
                    content_.clear();

                    auto row = std::make_shared<csv_row>();
                    string str_field;

                    auto& text = csv_string;
                    // 设置初始状态
                    state_type state = state_type::new_field_start;

                    for (size_t i = 0; i < text.size(); ++i)
                    {
                        const char& ch = text[i];
                        switch (state)
                        {
                        case state_type::new_field_start:
                        {
                            // 新字段开始
                            if (ch == '"')
                            {
                                state = state_type::quotes_field;
                            }
                            else if (ch == ',')
                            {
                                row->push_back("");
                                state = state_type::field_separator;
                            }
                            else if (ch == '\r' || ch == '\n')
                            {
                                //m_strErrorInfo = "语法错误：有空行";
                                //state = state_type::error;
                                throw csv_exception(csv_error::has_empty_line, "syntax error: has empty line");
                            }
                            else
                            {
                                str_field.push_back(ch);
                                state = state_type::non_quotes_field;
                            }
                        }
                        break;

                        case state_type::non_quotes_field:
                        {
                            // 非引号字段
                            if (ch == ',')
                            {
                                row->push_back(str_field);
                                str_field.clear();
                                state = state_type::field_separator;
                            }
                            else if (ch == '\r')
                            {
                                row->push_back(str_field);
                                state = state_type::row_separator;
                            }
                            else if (ch == '\n')
                            {
                                row->push_back(str_field);
                                content_.push_back(row);
                                row = std::make_shared<csv_row>();
                                str_field.clear();
                                state = state_type::new_field_start;
                            }
                            else
                            {
                                str_field.push_back(ch);
                            }
                        }
                        break;

                        case state_type::quotes_field:
                        {
                            // 引号字段
                            if (ch == '"')
                            {
                                state = state_type::quote_in_quotes_field;
                            }
                            else if (ch == '\r')
                            {
                                state = state_type::newline_in_quote_field;
                            }
                            else
                            {
                                str_field.push_back(ch);
                            }
                        }
                        break;

                        case state_type::field_separator:
                        {
                            // 字段分隔
                            if (ch == ',')
                            {
                                row->push_back("");
                            }
                            else if (ch == '"')
                            {
                                str_field.clear();
                                state = state_type::quotes_field;
                            }
                            else if (ch == '\r')
                            {
                                row->push_back("");
                                state = state_type::row_separator;
                            }
                            else if (ch == '\n')
                            {
                                row->push_back("");
                                content_.push_back(row);
                                row = std::make_shared<csv_row>();
                                str_field.clear();
                                state = state_type::new_field_start;
                            }
                            else
                            {
                                str_field.push_back(ch);
                                state = state_type::non_quotes_field;
                            }
                        }
                        break;

                        case state_type::quote_in_quotes_field:
                        {
                            // 引号字段中的引号
                            if (ch == ',')
                            {
                                // 引号字段闭合
                                row->push_back(str_field);
                                str_field.clear();
                                state = state_type::field_separator;
                            }
                            else if (ch == '\r')
                            {
                                // 引号字段闭合
                                row->push_back(str_field);
                                state = state_type::row_separator;
                            }
                            else if (ch == '\n')
                            {
                                row->push_back(str_field);
                                content_.push_back(row);
                                row = std::make_shared<csv_row>();
                                str_field.clear();
                                state = state_type::new_field_start;
                            }
                            else if (ch == '"')
                            {
                                // 转义
                                str_field.push_back(ch);
                                state = state_type::quotes_field;
                            }
                            else
                            {
                                /*m_strErrorInfo = "语法错误： 转义字符 \" 不能完成转义 或 引号字段结尾引号没有紧贴字段分隔符";
                                state = state_type::error;*/
                                throw csv_exception(csv_error::syntax_error, "syntax error: can't escape character \" complete escape or quotes ending quote without close to field delimiter");
                            }
                        }
                        break;

                        case state_type::row_separator:
                        {
                            // 行分隔符字符1，回车
                            if (ch == '\n')
                            {
                                content_.push_back(row);
                                row = std::make_shared<csv_row>();
                                str_field.clear();
                                state = state_type::new_field_start;
                            }
                            else
                            {
                                /*m_strErrorInfo = "语法错误： 行分隔用了回车 \\r。但未使用回车换行 \\r\\n ";
                                state = state_type::error;*/
                                throw csv_exception(csv_error::invalid_newline_character, "syntax error: line separated with a carriage return \\r.But they did not use carriage returns \\r\\n");
                            }
                        }
                        break;

                        case state_type::newline_in_quote_field:
                        {
                            if (ch == '\n')
                            {
                                str_field.push_back('\n');
                                state = state_type::quotes_field;
                            }
                            else
                            {
                                throw csv_exception(csv_error::invalid_newline_character, "syntax error: line separated with a carriage return \\r.But they did not use carriage returns \\r\\n");
                            }
                        }
                        break;

                        default: break;
                        }
                    }
                    // end for


                    switch (state)
                    {
                    case state_type::new_field_start:
                    {
                        // Excel导出的CSV每行都以/r/n结尾。包括最后一行
                    }
                    break;

                    case state_type::non_quotes_field:
                    {
                        row->push_back(str_field);
                        content_.push_back(row);
                    }
                    break;

                    case state_type::quotes_field:
                    {
                        //m_strErrorInfo = "语法错误： 引号字段未闭合";
                        throw csv_exception(csv_error::syntax_error, "syntax error: quotes field is not closed");
                    }
                    break;

                    case state_type::field_separator:
                    {
                        row->push_back("");
                        content_.push_back(row);
                    }
                    break;

                    case state_type::quote_in_quotes_field:
                    {
                        row->push_back(str_field);
                        content_.push_back(row);
                    }
                    break;

                    case state_type::row_separator:
                    {
                    }
                    break;

                    default: break;
                    }

                    set_header();
                }
                void parse_file(const string& file_name)
                {
                    ifstream file_stream;
                    file_stream.open(file_name, std::ios::in | std::ios::binary);
                    file_stream.seekg(0, std::ios::end);
                    size_t length = (size_t)file_stream.tellg();
                    file_stream.seekg(0, std::ios::beg);
                    if (length == 0)
                        return;
                    if (length > 50 * 1024 * 1024)
                        throw csv_exception(csv_error::over_max_size, "this file over the max size 50M");

                    vector<char> buffer(length);
                    file_stream.read(buffer.data(), buffer.size());
                    file_stream.close();

                    string content(buffer.data(), buffer.size());
                    buffer.clear();
                    parse_string(content);
                }

                const csv_row& operator[](size_t index) const
                {
                    if (index < row_size())
                        return *content_[index];

                    throw csv_exception(csv_error::index_overflow, "index overflowed the row size");
                }

                size_t row_size() const { return content_.size(); }
                size_t column_size() const { return header_.size(); }

                const vector<string>& header() const { return header_; }
            private:
                void set_header()
                {
                    if (!first_line_is_header_)
                        return;
                    header_.clear();
                    if (content_.empty())
                        return;

                    auto tmp_row = content_[0];
                    for (size_t i = 0; i < tmp_row->size(); ++i)
                    {
                        header_.push_back((*tmp_row)[i]);
                    }

                    if (!header_is_in_rows_)
                        content_.erase(content_.begin());

                    for (size_t i = 0; i < content_.size(); ++i)
                    {
                        content_[i]->set_header(&header_);
                    }
                }
            private:
                bool first_line_is_header_ = true;
                bool header_is_in_rows_ = false;
                vector<std::shared_ptr<csv_row>> content_;
                vector<string> header_;
            };
        }
    }

    using csv_error = parser::detail::csv_error;
    using csv_row = parser::detail::csv_row;
    using csv_exception = parser::detail::csv_exception;
    using csv = parser::detail::csv;
}
