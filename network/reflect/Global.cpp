#include "Global.h"

namespace cytx
{
    namespace meta
    {
        Global::Global(void)
            : type_( Type::Invalid( ) )
            , parentType_( Type::Invalid( ) )
            , name_( "INVALID" )
            , getter_( nullptr )
            , setter_( nullptr ) { }

        Global::Global(
            const std::string &name,
            Type type,
            GlobalGetterBase *getter,
            GlobalSetterBase *setter,
            Type parentType
        )
            : type_( type )
            , parentType_( parentType )
            , name_( name )
            , getter_( getter )
            , setter_( setter ) { }

        const Global &Global::Invalid(void)
        {
            static Global global;

            return global;
        }

        bool Global::IsValid(void) const
        {
            return getter_ != nullptr;
        }

        bool Global::IsReadOnly(void) const
        {
            return setter_ == nullptr;
        }

        Type Global::GetType(void) const
        {
            return type_;
        }

        Type Global::GetParentType(void) const
        {
            return parentType_;
        }

        const std::string &Global::GetName(void) const
        {
            return name_;
        }

        Variant Global::GetValue(void) const
        {
            return getter_->GetValue( );
        }

        bool Global::SetValue(const Argument &value) const
        {
            // read only?
            if (setter_ != nullptr)
            {
                setter_->SetValue( value );

                return true;
            }

            return false;
        }
    }
}
