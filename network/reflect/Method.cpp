#include "Method.h"

namespace cytx
{
    namespace meta
    {
        Method::Method(void)
            : Invokable( )
            , isConst_( true )
            , classType_( Type::Invalid( ) )
            , invoker_( nullptr ) { }

        const Method &Method::Invalid(void)
        {
            static Method invalid;

            return invalid;
        }

        Type Method::GetClassType(void) const
        {
            return classType_;
        }

        bool Method::IsValid(void) const
        {
            return invoker_ != nullptr;
        }

        bool Method::IsConst(void) const
        {
            return isConst_;
        }

        Variant Method::Invoke(
            Variant &instance,
            ArgumentList &arguments
        ) const
        {
        #if defined(_DEBUG)

            /*UAssert( IsValid( ),
                "Invalid method invoked."
            );

            UAssert( !(instance.IsConst( ) && !m_isConst),
                "Non-const method invoked on const object."
            );

            UAssert( instance.GetType( ) == m_classType,
                "Incompatible method invoked with instance."
            );*/

        #endif

            return invoker_->Invoke( instance, arguments );
        }
    }
}
