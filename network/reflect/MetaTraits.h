#pragma once
#include <type_traits>
#include <vector>
#include <unordered_map>
#include "Array.h"

namespace cytx
{
    namespace meta
    {
        template<typename T, typename = void>
        struct TypeOrEnumType
        {
            using type = T;
        };

        template<typename T>
        struct TypeOrEnumType<T, typename std::enable_if<std::is_enum<T>::value>::type>
        {
            using type = typename std::underlying_type<T>::type;
        };

        ///////////////////////////////////////////////////////////////////////////

        template<typename T>
        struct RemoveArray
        {
            using type = T;
        };

        template<typename T>
        struct RemoveArray<Array<T>>
        {
            using type = T;
        };

        ///////////////////////////////////////////////////////////////////////////

        template<typename T>
        struct IsArray
        {
            static const bool value = false;
        };

        template<typename T>
        struct IsArray<Array<T>>
        {
            static const bool value = true;
        };

        ///////////////////////////////////////////////////////////////////////////

        template<typename T>
        using ArrayByValue = Array<T>;

        template<typename T>
        using ArrayByReference = typename std::add_lvalue_reference<Array<T>>::type;

        template<typename T>
        using CleanedType = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

        template<typename T>
        using DecayType = CleanedType<typename RemoveArray<T>::type>;


        class Type;
        using InvokableSignature = std::vector<Type>;

        template<typename T>
        using InvokableOverloadMap = std::unordered_multimap<InvokableSignature, T>;

        // maximum number of arguments supported
        const size_t MaxArgumentCount = 10;

        class Argument;
        using ArgumentList = std::vector<Argument>;
    }
}