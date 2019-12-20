#include "common.h"
#include <network/util/string.hpp>

using namespace cytx;

TEST(string_util, empty_string)
{
    assert_true(util::is_empty_or_white_spate(""));
    assert_true(util::is_empty_or_white_spate(" "));
    assert_true(util::is_empty_or_white_spate("     "));
    assert_false(util::is_empty_or_white_spate("a"));
}

TEST(string_util_split_by_chars, one_char)
{
    auto strs = string_util::split("a,b", ",");
    assert_eq(strs.size(), 2);
    assert_streq(strs[0].c_str(), "a");
    assert_streq(strs[1].c_str(), "b");

    auto strs1 = string_util::split("a b", ",");
    assert_eq(strs1.size(), 1);
    assert_streq(strs1[0].c_str(), "a b");
}