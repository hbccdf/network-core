#pragma once
#include "ArrayWrapperBase.h"

namespace cytx
{
    namespace meta
    {
        class Variant;
        class Argument;

        template<typename T>
        class ArrayWrapperContainer : public ArrayWrapperBase
        {
        public:
            ArrayWrapperContainer(Array<T>& a);

            Variant GetValue(size_t index) override;
            void SetValue(size_t index, const Argument& value) override;

            void Insert(size_t index, const Argument& value) override;
            void Remove(size_t index) override;

            size_t Size(void) const override;

        private:
            Array<T>& array_;
        };
    }
}


#include "Argument.h"
#include "Variant.h"

namespace cytx
{
    namespace meta
    {
        template<typename T>
        ArrayWrapperContainer<T>::ArrayWrapperContainer(Array<T> &a)
            : array_(a)
        {

        }

        template<typename T>
        Variant ArrayWrapperContainer<T>::GetValue(size_t index)
        {
            return{ array_[index] };
        }

        template<typename T>
        void ArrayWrapperContainer<T>::SetValue(size_t index, const Argument &value)
        {
            array_.at(index) = value.GetValue<T>();
        }

        template<typename T>
        void ArrayWrapperContainer<T>::Insert(size_t index, const Argument &value)
        {
            array_.insert(array_.begin() + index, value.GetValue<T>());
        }

        template<typename T>
        void ArrayWrapperContainer<T>::Remove(size_t index)
        {
            array_.erase(array_.begin() + index);
        }

        template<typename T>
        size_t ArrayWrapperContainer<T>::Size(void) const
        {
            return array_.size();
        }
    }
}