#include "Enum.h"
#include "Variant.h"

namespace cytx
{
    namespace meta
    {
        Enum::Enum(const EnumBase *base)
            : base_( base ) { }

        bool Enum::IsValid(void) const
        {
            return base_ != nullptr;
        }

        Enum::operator bool(void) const
        {
            return base_ != nullptr;
        }

        bool Enum::operator==(const Enum &rhs) const
        {
            return base_ == rhs.base_;
        }

        bool Enum::operator!=(const Enum &rhs) const
        {
            return base_ != rhs.base_;
        }

        std::string Enum::GetName(void) const
        {
            return base_ ? base_->GetName( ) : std::string( );
        }

        Type Enum::GetType(void) const
        {
            return base_ ? base_->GetType( ) : Type::Invalid( );
        }

        Type Enum::GetParentType(void) const
        {
            return base_ ? base_->GetParentType( ) : Type::Invalid( );
        }

        Type Enum::GetUnderlyingType(void) const
        {
            return base_ ? base_->GetUnderlyingType( ) : Type::Invalid( );
        }

        std::vector<std::string> Enum::GetKeys(void) const
        {
            return base_ ? base_->GetKeys( ) : std::vector<std::string>( );
        }

        std::vector<Variant> Enum::GetValues(void) const
        {
            return base_ ? base_->GetValues( ) : std::vector<Variant>( );
        }

        std::string Enum::GetKey(const Argument &value) const
        {
            return base_ ? base_->GetKey( value ) : std::string( );
        }

        Variant Enum::GetValue(const std::string &key) const
        {
            return base_ ? base_->GetValue( key ) : Variant( );
        }
    }
}
