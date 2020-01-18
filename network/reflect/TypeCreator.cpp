#include "TypeCreator.h"

namespace cytx
{
    namespace meta
    {
        Variant TypeCreator::CreateVariadic(const Type &type, const ArgumentList &arguments)
        {
            InvokableSignature signature;

            for (auto &argument : arguments)
                signature.emplace_back( argument.GetType( ) );

            auto &constructor = type.GetConstructor( signature );

            return constructor.InvokeVariadic( arguments );
        }

        Variant TypeCreator::CreateDynamicVariadic(const Type &type, const ArgumentList &arguments)
        {
            InvokableSignature signature;

            for (auto &argument : arguments)
                signature.emplace_back( argument.GetType( ) );

            auto &constructor = type.GetDynamicConstructor( signature );

            return constructor.InvokeVariadic( arguments );
        }
    }
}