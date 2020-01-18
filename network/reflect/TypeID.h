#pragma once
#include <cstdint>
#include "MetaTraits.h"

namespace cytx
{
    namespace meta
    {
        using TypeID = uint32_t;

        const TypeID InvalidTypeID = 0;

        template<typename T>
        struct TypeIDs
        {
            static TypeID ID;
        };

        template<typename T>
        TypeID TypeIDs<T>::ID = InvalidTypeID;
    }
}


// Gets the type ID of a given expression
#define typeidof(expr) cytx::meta::TypeIDs<cytx::meta::DecayType<expr>>::ID

// Converts the expression into a meta::Type instance
#define typeof(expr) cytx::meta::Type(typeidof(expr), cytx::meta::IsArray<expr>::value)

// Converts the resulting type of the given expression to a meta::Type instance
#define decltypeof(expr) typeof(decltype(expr))