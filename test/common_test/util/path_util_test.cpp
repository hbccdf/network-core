#include "common.h"
#include <network/util/path.hpp>

using namespace cytx;

TEST(path_util, empty_string)
{
    assert_true(path_util::match_path("", ""));
    assert_false(path_util::match_path("./", ""));
    assert_true(path_util::match_path("./", "./"));
    assert_true(path_util::match_path(".", "."));
}

TEST(path_util, cur_dir)
{
    assert_true(path_util::match_path("./", "./"));
    assert_true(path_util::match_path("./cache", "cache"));
    assert_true(path_util::match_path("cache", "./cache"));
    assert_false(path_util::match_path("./cache", ".cache"));
    assert_false(path_util::match_path(".cache", "./cache"));

    assert_true(path_util::match_path("./test/show", "test/show"));
    assert_true(path_util::match_path("test/show", "./test/show"));
    assert_false(path_util::match_path("../test/show", "test/show"));
    assert_false(path_util::match_path("test/show", "../test/show"));
}

TEST(path_util, files)
{
    assert_true(path_util::match_path("/test/show/3.map", "/test/show/3.map"));
    assert_true(path_util::match_path("/test/show/3.map/", "/test/show/3.map/"));

    assert_true(path_util::match_path("test/show/3.map", "test/show/3.map"));
    assert_true(path_util::match_path("test/show/3.map/", "test/show/3.map/"));

    assert_false(path_util::match_path("/test/show/3.map", "test/show/3.map"));
    assert_false(path_util::match_path("/test/show/3.map/", "test/show/3.map/"));

    assert_false(path_util::match_path("test/show/3.map", "/test/show/3.map"));
    assert_false(path_util::match_path("test/show/3.map/", "/test/show/3.map/"));

    assert_false(path_util::match_path("/test/show/3.map", "/test/1/show/3.map"));
    assert_false(path_util::match_path("/test/show/3.map", "/3/test/show/3.map"));

    assert_false(path_util::match_path("/3.map/show/4.map", "/3.map"));
    assert_false(path_util::match_path("/3.map", "/3.map/test/show/4.map"));

    assert_false(path_util::match_path("/3.map/show/4.map", "/3.map", true));
    assert_false(path_util::match_path("/3.map", "/3.map/test/show/4.map", true));
}

TEST(path_util, simple_glob)
{
    assert_true(path_util::match_path("/test/show/3.map", "/test/show/*.map"));
    assert_true(path_util::match_path("/test/show/3.map", "/test/show/*.m*"));
    assert_false(path_util::match_path("/test/show/3.map", "/test/show/*.m"));

    assert_true(path_util::match_path("/test/show/3.map", "/test/show/*.*"));
    assert_true(path_util::match_path("/test/show/3.map", "/test/show/*"));

    assert_true(path_util::match_path("/test/show/3.map", "/test/*/3.map"));
    assert_true(path_util::match_path("/test/show/3.map", "/*/show/3.map"));
    assert_true(path_util::match_path("/test/show/3.map", "/test/show/?.map"));
    assert_true(path_util::match_path("/test/show/3.map", "/test/show/3.???"));

    assert_false(path_util::match_path("/test/show/3.map", "/test/*/te/3.map"));
}

TEST(path_util, glob)
{
    assert_true(path_util::match_path("/test/show/hello/3.map", "**/*.map"));
    assert_true(path_util::match_path("/test/show/hello/3.map", "/**/*.map"));
    assert_true(path_util::match_path("/test/show/hello/3.map", "**"));
    assert_false(path_util::match_path("/test/show/hello/3.map", "/test/**/show/3.map"));
    assert_true(path_util::match_path("/test/show/hello/show/3.map", "/test/**/show/3.map"));

    assert_true(path_util::match_path("/test/show/hello/show/test/show/hello/3.map", "/test/**/show/**/hello/3.map"));
    assert_false(path_util::match_path("/test/show/hello/world/test/show/hello/3.map", "/test/**/show/**/hello/3.map"));
    assert_false(path_util::match_path("/test/show/hello/3.map", "/test/**/show/**/hello/3.map"));
    assert_true(path_util::match_path("/test/1/show/hello/hello/3.map", "/test/**/show/**/hello/3.map"));

    assert_false(path_util::match_path("/test/show/hello/show/test/show/hello/3.map", "/test/**hello/show/**/hello/3.map"));
    assert_true(path_util::match_path("/test/1/show/hello/hello/3.map", "/test/**/show/**h*/hello/3.map"));
    assert_false(path_util::match_path("/test/1/show/hello/hello/hello/3.map", "/test/**/show/**h*/hello/3.map"));



    assert_false(path_util::match_path("./test.exe", "**/*.txt"));
    assert_true(path_util::match_path("./test.exe", "**/*.txt", true));
}

TEST(path_util, glob_dir)
{

}