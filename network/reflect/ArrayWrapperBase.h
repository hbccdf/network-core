#pragma once
#include "Array.h"

namespace cytx
{
    namespace meta
    {
        class Variant;
        class Argument;

        class ArrayWrapperBase
        {
        public:
            virtual ~ArrayWrapperBase() { }

            virtual Variant GetValue(size_t index) = 0;
            virtual void SetValue(size_t index, const Argument& value) = 0;

            virtual void Insert(size_t index, const Argument& value) = 0;
            virtual void Remove(size_t index) = 0;

            virtual size_t Size() const = 0;
        };
    }
}