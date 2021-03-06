#pragma once
#include "Variant.h"

namespace cytx
{
    namespace meta
    {
        class TypeCreator
        {
        public:
            /** @brief Instantiates an instance of the given type with the given
             *         constructor signature. NOTE: it is much faster to cache
             *         the appropriate constructor first, then call
             *         Invoke( ) manually.
             *  @param arguments List of arguments to forward to the
             *                   type constructor.
             *  @return Variant representing the newly created type instance.
             */
            static Variant CreateVariadic(const Type& type, const ArgumentList& arguments);

            /** @brief Same as CreateVariadic( ), except it uses the dynamic
             *         constructor and returns the class pointer type.
             *  @param arguments List of arguments to forward to the
             *                   type constructor.
             *  @return Variant representing a pointer to the newly
             *          created type instance. NOTE: client is responsible for
             *          memory management, either through type.Destroy( ) or
             *          directly calling the underlying pointer's deconstructor
             */
            static Variant CreateDynamicVariadic(const Type& type, const ArgumentList& arguments);

            /** @brief Instantiates an instance of the type with the given
             *         constructor signature. NOTE: it is much faster to cache
             *         the appropriate constructor first, then call
             *         Invoke( ) manually.
             *  @param args List of arguments to forward to the
             *                   type constructor.
             *  @return Variant representing the newly created type instance.
             */
            template<typename ...Args>
            static Variant Create(const Type& type, Args &&...args);

            /** @brief Same as Create( ), except it uses the dynamic
              *         constructor and returns the class pointer type.
              * @param args List of arguments to forward to the
              *                   type constructor.
              * @return Variant representing a pointer to the newly
              *          created type instance. NOTE: client is responsible for
              *          memory management, either through type.Destroy( ) or
              *          directly calling the underlying pointer's deconstructor
              */
            template<typename ...Args>
            static Variant CreateDynamic(const Type& type, Args &&...args);
        };
    }
}


#include "Constructor.h"

namespace cytx
{
    namespace meta
    {
        template<typename ...Args>
        Variant TypeCreator::Create(const Type &type, Args &&...args)
        {
            static InvokableSignature signature;
            static bool initial = true;

            if (initial)
            {
                TypeUnpacker<Args...>::Apply(signature);
                initial = false;
            }

            auto &constructor = type.GetConstructor(signature);
            return constructor.Invoke(std::forward<Args>(args)...);
        }

        template<typename ...Args>
        Variant TypeCreator::CreateDynamic(const Type &type, Args &&...args)
        {
            static InvokableSignature signature;
            static bool initial = true;

            if (initial)
            {
                TypeUnpacker<Args...>::Apply(signature);
                initial = false;
            }

            auto &constructor = type.GetDynamicConstructor(signature);
            return constructor.Invoke(std::forward<Args>(args)...);
        }
    }
}