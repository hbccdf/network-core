#include "Function.h"

namespace cytx
{
    namespace meta
    {
        Function::Function(void)
            : Invokable( )
            , parentType_( Type::Invalid( ) )
            , invoker_( nullptr ) { }

        const Function &Function::Invalid(void)
        {
            static Function invalid;

            return invalid;
        }

        Type Function::GetParentType(void) const
        {
            return parentType_;
        }

        bool Function::IsValid(void) const
        {
            return invoker_ != nullptr;
        }

        Variant Function::InvokeVariadic(ArgumentList &arguments) const
        {
        #if defined(_DEBUG)

            //UAssert( IsValid( ), "Invalid function invocation." );

        #endif

            return invoker_->Invoke( arguments );
        }
    }
}
