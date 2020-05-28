#pragma once
#include <fstream>
#include <vector>
#include <fmt/format.h>
#include "network/util/string.hpp"

namespace cytx
{
    namespace util
    {
        using memory_wirter_t = fmt::MemoryWriter;

        class file_util
        {
        public:
            static int64_t size(const std::string& file_path)
            {
                std::ifstream stream;
                stream.open(file_path, std::ios::in | std::ios::binary);
                stream.seekg(0, std::ios::end);
                return (int64_t)stream.tellg();
            }

            static std::string read_all(const std::string& file_path)
            {
                std::string content;
                std::ifstream stream;
                stream.open(file_path, std::ios::in | std::ios::binary);
                if (!stream.is_open())
                    return content;

                stream.seekg(0, std::ios::end);
                size_t length = (size_t)stream.tellg();
                stream.seekg(0, std::ios::beg);
                if (length == 0)
                    return content;

                content.resize(length);
                stream.read((char*)content.data(), content.size());
                stream.close();

                return content;
            }

            static std::vector<std::string> read_lines(const std::string& file_path)
            {
                std::string content = read_all(file_path);
                return string_util::split(content, "\r\n");
            }

            static void write_all(const std::string& file_path, const std::string& content)
            {
                write_all(file_path, content.data(), content.length());
            }

            static void write_all(const std::string& file_path, const char* buffer, size_t buffer_size)
            {
                std::ofstream stream;
                stream.open(file_path, std::ios::out | std::ios::binary);
                stream.write(buffer, buffer_size);
            }

            static void append_all(const std::string& file_path, const std::string& content)
            {
                write_all(file_path, content.data(), content.length());
            }

            static void append_all(const std::string& file_path, const char* buffer, size_t buffer_size)
            {
                std::ofstream stream;
                stream.open(file_path, std::ios::out | std::ios::binary | std::ios::app);
                stream.write(buffer, buffer_size);
            }

        public:
            static void windows_to_unix(const std::string& file_path)
            {
                std::string content = read_all(file_path);
                string_util::replace(content, "\r\n", "\n");
            }

            static void unix_to_windows(const std::string& file_path)
            {
                memory_wirter_t mr;
                std::string content = read_all(file_path);
                char prev_char = 0;
                for (size_t i = 0; i < content.size(); ++i)
                {
                    if (prev_char != '\r' && content[i] == '\n')
                    {
                        mr << '\r' << '\n';
                    }
                    else
                    {
                        mr << content[i];
                    }
                    prev_char = content[i];
                }

                write_all(file_path, mr.str());
            }
        };
    }

    using file_util = util::file_util;
}