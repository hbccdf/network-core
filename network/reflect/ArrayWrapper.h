#pragma once
#include "ArrayWrapperBase.h"

namespace cytx
{
    namespace meta
    {
        class Argument;

        class ArrayWrapper
        {
        public:
            ArrayWrapper();

            template<typename T>
            ArrayWrapper(Array<T>& rhs);

            template<typename T>
            ArrayWrapper(const Array<T>& rhs);

            Variant GetValue(size_t index) const;
            void SetValue(size_t index, const Argument& value);

            void Insert(size_t index, const Argument& value);
            void Remove(size_t index);

            size_t Size() const;

            bool IsValid() const;
            bool IsConst() const;
        private:
            bool isConst_;

            ArrayWrapperBase* base_;
        };
    }
}

#include "ArrayWrapperContainer.h"
namespace cytx
{
    namespace meta
    {
        template<typename T>
        ArrayWrapper::ArrayWrapper(Array<T> &rhs)
            : isConst_(false)
            , base_(new ArrayWrapperContainer<T>(rhs))
        {

        }

        template<typename T>
        ArrayWrapper::ArrayWrapper(const Array<T> &rhs)
            : isConst_(true)
            , base_(new ArrayWrapperContainer<T>(const_cast<Array<T> &>(rhs)))
        {

        }
    }
}