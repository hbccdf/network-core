#pragma once
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace cytx
{
    namespace util
    {
        namespace bfs = boost::filesystem;

        class path_util
        {
        public:
            static std::vector<std::string> get_files(const std::string& dir)
            {
                std::vector<std::string> list;
                get_files(list, dir);
            }

            static void get_files(std::vector<std::string>& files, const std::string& dir)
            {
                bfs::directory_iterator it(dir);
                bfs::directory_iterator end_it;
                while (it != end_it)
                {
                    if (bfs::is_directory(*it))
                    {
                        ++it;
                        continue;
                    }

                    files.push_back(it->path);
                }
            }
        };
    }
}