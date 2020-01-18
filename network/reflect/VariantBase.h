#pragma once
#include <string>

namespace cytx
{
    namespace meta
    {
        namespace variant_policy
        {
            /** @brief Wraps an Object pointer, rather than copying the value.
            *         This also ensures base class pointers can resolve their inherited type
            */
            struct WrapObject { };

            /** @brief Uses a reference to a value rather than copying it
            */
            struct NoCopy { };
        }

        class Type;
        class ArrayWrapper;

        class VariantBase
        {
        public:
            virtual ~VariantBase() {}

            virtual Type GetType() const = 0;

            virtual void* GetPtr() const = 0;

            virtual int ToInt() const = 0;

            virtual bool ToBool() const = 0;

            virtual float ToFloat() const = 0;

            virtual double ToDouble() const = 0;

            virtual std::string ToString() const = 0;

            virtual bool IsArray() const { return false; }

            virtual ArrayWrapper GetArray() const;

            virtual VariantBase* Clone() const = 0;
        };
    }
}