#include "Field.h"
#include "Method.h"

namespace cytx
{
    namespace meta
    {
        Field::Field(void)
            : type_( Type::Invalid( ) )
            , classType_( Type::Invalid( ) )
            , name_( "INVALID" )
            , getter_( nullptr )
            , setter_( nullptr ) { }

        Field::Field(
            const std::string &name,
            Type type,
            Type classType,
            FieldGetterBase *getter,
            FieldSetterBase *setter
        )
            : type_( type )
            , classType_( classType )
            , name_( name )
            , getter_( getter )
            , setter_( setter ) { }

        const Field &Field::Invalid(void)
        {
            static Field field;

            return field;
        }

        bool Field::SetValue(Variant &instance, const Variant &value, const Method &setter)
        {
             // read only?
            if (!instance.IsConst( ))
            {
                setter.Invoke( instance, value );

                return true;
            }

            return false;
        }

        bool Field::IsValid(void) const
        {
            return getter_ != nullptr;
        }

        bool Field::IsReadOnly(void) const
        {
            return setter_ == nullptr;
        }

        Type Field::GetType(void) const
        {
            return type_;
        }

        Type Field::GetClassType(void) const
        {
            return classType_;
        }

        const std::string &Field::GetName(void) const
        {
            return name_;
        }

        Variant Field::GetValue(const Variant &instance) const
        {
            return getter_->GetValue( instance );
        }

        Variant Field::GetValueReference(const Variant &instance) const
        {
            return getter_->GetValueReference( instance );
        }

        bool Field::SetValue(Variant &instance, const Variant &value) const
        {
            // read only?
            if (setter_ && !instance.IsConst( ))
            {
                setter_->SetValue( instance, value );

                return true;
            }

            return false;
        }
    }
}
