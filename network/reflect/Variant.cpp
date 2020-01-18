#include "Variant.h"
#include "Type.h"

namespace cytx
{
    namespace meta
    {
        Variant::Variant(void)
            : isConst_( true )
            , base_( nullptr ) { }

        Variant::Variant(const Variant &rhs)
            : isConst_( rhs.isConst_ )
            , base_( rhs.base_ ? rhs.base_->Clone( ) : nullptr ) { }

        Variant::Variant(Variant &&rhs)
            : isConst_( rhs.isConst_ )
            , base_( rhs.base_ )
        {
            rhs.isConst_ = true;
            rhs.base_ = nullptr;
        }

        Variant::~Variant(void)
        {
            delete base_;
        }

        Variant &Variant::operator=(const Variant &rhs)
        {
            Variant( rhs ).Swap( *this );

            return *this;
        }

        Variant &Variant::operator=(Variant &&rhs)
        {
            rhs.Swap( *this );

            Variant( ).Swap( rhs );

            return *this;
        }

        Variant::operator bool(void) const
        {
            return base_ != nullptr;
        }

        Type Variant::GetType(void) const
        {
            return base_ ? base_->GetType( ) : Type::Invalid( );
        }

        ArrayWrapper Variant::GetArray(void) const
        {
            return base_ ? base_->GetArray( ) : ArrayWrapper( );
        }

        int Variant::ToInt(void) const
        {
            return base_ ? base_->ToInt( ) : int( );
        }

        bool Variant::ToBool(void) const
        {
            return base_ ? base_->ToBool( ) : bool( );
        }

        float Variant::ToFloat(void) const
        {
            return base_ ? base_->ToFloat( ) : float( );
        }

        double Variant::ToDouble(void) const
        {
            return base_ ? base_->ToDouble( ) : double( );
        }

        std::string Variant::ToString(void) const
        {
            return base_ ? base_->ToString( ) : std::string( );
        }

        void Variant::Swap(Variant &other)
        {
            std::swap( base_, other.base_ );
        }

        bool Variant::IsValid(void) const
        {
            return base_ != nullptr;
        }

        bool Variant::IsConst(void) const
        {
            return isConst_;
        }

        bool Variant::IsArray(void) const
        {
            return base_ ? base_->IsArray( ) : false;
        }

        void *Variant::getPtr(void) const
        {
            return base_ ? base_->GetPtr( ) : nullptr;
        }
    }
}
