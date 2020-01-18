#pragma once
#include <vector>
#include "TypeID.h"

namespace cytx
{
    namespace meta
    {
        class Type;
        class Variant;

        class Argument
        {
        public:
            Argument();
            Argument(const Argument& rhs);
            Argument(Variant& obj);
            Argument(const Variant& obj);

            template<typename T>
            Argument(const T& data);

            template<typename T>
            Argument(T& data);

            Argument& operator=(const Argument& rhs);

            Type GetType() const;

            void *GetPtr() const;

            template<typename T>
            T &GetValue() const;

        private:
            const TypeID typeID_;
            const bool isArray_;

            const void* data_;
        };
    }
}


namespace cytx
{
    namespace meta
    {
        template<typename T>
        Argument::Argument(const T &data)
            : typeID_(typeidof(T))
            , isArray_(IsArray<T>::value)
            , data_(reinterpret_cast<const void*>(std::addressof(data)))
        {
            static_assert(!std::is_same< Argument, T >::value,
                "Cannot use Argument as an argument");
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename T>
        Argument::Argument(T &data)
            : typeID_(typeidof(T))
            , isArray_(meta_traits::IsArray<T>::value)
            , data_(reinterpret_cast<const void*>(std::addressof(data)))
        {
            static_assert(!std::is_same< Argument, T >::value,
                "Cannot use Argument as an argument");
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename T>
        T &Argument::GetValue(void) const
        {
            return *reinterpret_cast<typename std::remove_reference<T>::type*>(const_cast<void *>(data_));
        }
    }
}