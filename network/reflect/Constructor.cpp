#include "Constructor.h"
#include "Invokable.h"
#include "Type.h"

namespace cytx
{
    namespace meta
    {
        namespace
        {
            const auto kConstructorName = "constructor";
        }

        Constructor::Constructor(void)
            : Invokable( )
            , isDynamic_( false )
            , classType_( InvalidTypeID )
            , invoker_( nullptr ) { }

        Constructor::Constructor(const Constructor &rhs)
            : Invokable( kConstructorName )
            , isDynamic_( rhs.isDynamic_ )
            , classType_( rhs.classType_ )
            , invoker_( rhs.invoker_ )
        {
            signature_ = rhs.signature_;
        }

        Constructor::Constructor(const Constructor &&rhs) noexcept
	        : Invokable( kConstructorName )
            , isDynamic_( rhs.isDynamic_ )
            , classType_( rhs.classType_ )
            , invoker_( std::move( rhs.invoker_ ) )
        {
            signature_ = std::move( rhs.signature_ );
        }

        Constructor::Constructor(
            Type classType,
            InvokableSignature signature,
            ConstructorInvokerBase *invoker,
            bool isDynamic
        )
            : Invokable( kConstructorName )
            , isDynamic_( isDynamic )
            , classType_( classType )
            , invoker_( invoker )
        {
            signature_ = signature;
        }

        Constructor &Constructor::operator=(const Constructor &&rhs)
        {
            isDynamic_ = rhs.isDynamic_;
            classType_= rhs.classType_;
            invoker_ = std::move( rhs.invoker_ );

            signature_ = std::move( rhs.signature_ );

            return *this;
        }

        const Constructor &Constructor::Invalid(void)
        {
            static Constructor invalid;

            return invalid;
        }

        Type Constructor::GetClassType(void) const
        {
            return classType_;
        }

        bool Constructor::IsValid(void) const
        {
            return invoker_ != nullptr;
        }

        bool Constructor::IsDynamic(void) const
        {
            return isDynamic_;
        }

        Variant Constructor::InvokeVariadic(const ArgumentList &arguments) const
        {
            //UAssert( IsValid( ), "Invalid constructor invoked." );

            return invoker_->Invoke( arguments );
        }
    }
}
