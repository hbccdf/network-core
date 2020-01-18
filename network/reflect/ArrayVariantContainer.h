#pragma once
#include "VariantBase.h"
#include "Array.h"

namespace cytx
{
    namespace meta
    {
        class ArrayWrapper;

        template<typename T, typename StorageType>
        class ArrayVariantContainer : public VariantBase
        {
        public:
            ArrayVariantContainer(StorageType& rhs)
                : array_(rhs) { }

            Type GetType() const override
            {
                return typeof(Array<T>);
            }
            void* GetPtr() const override
            {
                return const_cast<void*>(reinterpret_cast<const void*>(std::addressof(array_)));
            }

            int ToInt() const override { return int{}; }
            bool ToBool() const override { return bool{}; }
            float ToFloat() const override { return float{}; }
            double ToDouble() const override { return double{}; }
            std::string ToString() const override { return std::string{}; }

            bool IsArray() const override { return true; }
            ArrayWrapper GetArray() const override;

            VariantBase* Clone() const override
            {
                return new ArrayVariantContainer<T, StorageType>(const_cast<Array<T>&>(array_));
            }

        private:
            StorageType array_;
        };
    }
}


#include "ArrayWrapper.h"
namespace cytx
{
    namespace meta
    {
        template<typename T, typename StorageType>
        ArrayWrapper ArrayVariantContainer<T, StorageType>::GetArray(void) const
        {
            return ArrayWrapper(const_cast<Array<T>&>(array_));
        }
    }
}