#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <network/base/auto_mocker.h>

template<typename T>
void assert_val_equal(T&& v, T&& dv)
{
    assert_eq(v, dv);
}

template<>
inline void assert_val_equal<float>(float&& v, float&& dv)
{
    assert_float_eq(v, dv);
}
template<>
inline void assert_val_equal<double>(double&& v, double&& dv)
{
    assert_double_eq(v, dv);
}

template<typename T, size_t N>
void assert_val_equal(T(&v)[N], T(&dv)[N])
{
    for (size_t i = 0; i < N; ++i)
    {
        assert_val_equal(v[i], dv[i]);
    }
}

template<size_t N>
void assert_val_equal(char(&v)[N], char(&dv)[N])
{
    assert_streq(v, dv);
}