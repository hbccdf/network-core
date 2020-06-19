#pragma once
#include <stack>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <network/util/string.hpp>

namespace cytx
{
    namespace util
    {
        namespace bfs = boost::filesystem;

        class path_util
        {
        public:
            static bool exists(const std::string& filepath)
            {
                return bfs::exists(filepath);
            }

            static bool is_file(const std::string& filepath)
            {
                return bfs::is_regular_file(filepath);
            }

            static bool is_dir(const std::string& filepath)
            {
                return bfs::is_directory(filepath);
            }

            //sub_dir_depth递归深度，-1表示不限递归深度
            static std::vector<std::string> get_files(const std::string& dir, bool include_sub_dir = false, int sub_dir_depth = -1)
            {
                std::vector<std::string> list;
                get_files(list, dir, include_sub_dir);
                return list;
            }

            static void get_files(std::vector<std::string>& files, const std::string& dir, bool include_sub_dir = false, int sub_dir_depth = -1)
            {
                bfs::directory_iterator it(dir);
                bfs::directory_iterator end_it;
                while (it != end_it)
                {
                    const std::string& cur_path = it->path().string();
                    if (bfs::is_directory(*it))
                    {
                        if (include_sub_dir && sub_dir_depth > 0)
                        {
                            int depth = sub_dir_depth - 1;
                            get_files(files, cur_path, include_sub_dir, depth);
                        }
                        else
                        {
                            ++it;
                            continue;
                        }
                    }

                    if (bfs::is_regular_file(it->status()))
                    {
                        files.push_back(cur_path);
                    }
                    ++it;
                }
            }

            //sub_dir_depth递归深度，-1表示不限递归深度
            static std::vector<std::string> get_dirs(const std::string& dir, bool include_sub_dir = false, int sub_dir_depth = -1)
            {
                std::vector<std::string> list;
                get_dirs(list, dir, include_sub_dir);
                return list;
            }

            static void get_dirs(std::vector<std::string>& dirs, const std::string& dir, bool include_sub_dir = false, int sub_dir_depth = -1)
            {
                bfs::directory_iterator it(dir);
                bfs::directory_iterator end_it;
                while (it != end_it)
                {
                    const std::string& cur_path = it->path().string();
                    if (!bfs::is_directory(*it))
                    {
                        ++it;
                        continue;
                    }

                    dirs.push_back(cur_path);
                    if (include_sub_dir && sub_dir_depth > 0)
                    {
                        int depth = sub_dir_depth - 1;
                        get_dirs(dirs, cur_path, include_sub_dir, depth);
                    }
                    ++it;
                }
            }

            static std::vector<std::string> glob_paths(const std::string& dir, const std::string& glob, bool include_dir_path = false)
            {
                std::vector<std::string> paths;
                internal_glob_paths(paths, dir, dir, glob, include_dir_path);
                std::sort(paths.begin(), paths.end());
                return paths;
            }

            static void glob_paths(std::vector<std::string>& paths, const std::string& dir, const std::string& glob, bool include_dir_path = false)
            {
                if (paths.empty())
                {
                    internal_glob_paths(paths, dir, dir, glob, include_dir_path);
                    std::sort(paths.begin(), paths.end());
                }
                else
                {
                    std::vector<std::string> tmp_paths;
                    internal_glob_paths(tmp_paths, dir, dir, glob, include_dir_path);
                    std::sort(tmp_paths.begin(), tmp_paths.end());
                    std::copy(tmp_paths.begin(), tmp_paths.end(), std::back_inserter(paths));
                }
            }

            static bool match_path(const std::string& path_str, const std::string& glob_str, bool is_dir = false, bool ignore_case = true)
            {
                if (glob_str.empty() && !path_str.empty())
                    return false;

                if (path_str.empty() && !glob_str.empty())
                    return false;

                if (glob_str == path_str)
                    return true;

                std::vector<std::string> splited_path = string_util::split(path_str, "\\/");
                std::vector<std::string> splited_glob = string_util::split(glob_str, "\\/");

                if (splited_path.empty() && !splited_glob.empty())
                    return false;

                if (splited_path.empty() && !splited_glob.empty())
                    return false;

                if (splited_path.empty() && splited_glob.empty())
                    return true;

                size_t path_index = 0;
                size_t glob_index = 0;
                bool is_match = true;

                struct pg_info
                {
                    size_t pi;
                    size_t gi;
                };
                std::stack<pg_info> globs;

                while (path_index < splited_path.size() || glob_index < splited_glob.size())
                {
                    if (path_index >= splited_path.size() || glob_index >= splited_glob.size())
                    {
                        if ((is_dir && !globs.empty()) || !is_match)
                            break;

                        if (path_index >= splited_path.size())
                            path_index = splited_path.size() - 1;
                        if (glob_index >= splited_glob.size())
                            glob_index = splited_glob.size() - 1;
                    }

                    std::string& p = splited_path[path_index];
                    std::string& g = splited_glob[glob_index];

                    if (p == "." || g == ".")
                    {
                        if (p == ".")
                            ++path_index;
                        if (g == ".")
                            ++glob_index;

                        is_match = true;
                        continue;
                    }

                    if (g == "**")
                    {
                        //第一次
                        if (globs.empty())
                        {
                            pg_info pg;
                            pg.pi = path_index++;
                            pg.gi = glob_index++;
                            globs.push(pg);
                        }
                        else
                        {
                            //连续两个 **
                            pg_info prev_pg = globs.top();
                            if (prev_pg.gi + 1 == glob_index)
                            {
                                prev_pg.gi = glob_index++;
                                globs.pop();
                                globs.push(prev_pg);
                            }
                            else
                            {
                                pg_info pg;
                                pg.pi = path_index++;
                                pg.gi = glob_index++;
                                globs.push(pg);
                            }
                        }
                        is_match = true;
                        continue;
                    }

                    if (p == g || string_util::match(p, g, !ignore_case))
                    {
                        is_match = true;
                        path_index++;
                        glob_index++;
                        continue;
                    }

                    if(!globs.empty())
                    {
                        pg_info pg = globs.top();
                        globs.pop();
                        path_index = pg.pi + 1;
                        glob_index = pg.gi;
                        is_match = false;
                        continue;
                    }

                    return false;
                }

                /*if (is_match)
                {
                    if (!splited_path.empty() && !splited_glob.empty() && (path_index != splited_path.size()))
                    {
                        return false;
                    }
                }*/

                return is_match;
            }

        private:
            static void internal_glob_paths(std::vector<std::string>& paths, const std::string& root_dir, const std::string& dir, const std::string& glob, bool include_dir_path = false)
            {
                bfs::directory_iterator it(dir);
                bfs::directory_iterator end_it;
                while (it != end_it)
                {
                    const std::string& cur_path = it->path().string();
                    std::string relative_path = bfs::relative(cur_path, root_dir).string();

                    if (bfs::is_regular_file(it->status()))
                    {
                        if (match_path(relative_path, glob, false, true))
                            paths.push_back(cur_path);
                    }
                    else if (bfs::is_directory(*it))
                    {
                        if (match_path(relative_path, glob, true, true))
                        {
                            if (include_dir_path)
                                paths.push_back(cur_path);

                            internal_glob_paths(paths, root_dir, cur_path, glob, include_dir_path);
                        }
                    }

                    ++it;
                }
            }
        };
    }

    using path_util = util::path_util;
}