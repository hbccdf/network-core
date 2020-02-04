#include "EnumBase.h"
#include "Variant.h"
#include "Argument.h"

namespace cytx
{
    namespace meta
    {
        EnumBase::EnumBase(const std::string &name, TypeID owner)
            : parentType_( owner )
            , name_( name ) { }

        Type EnumBase::GetParentType(void) const
        {
            return parentType_;
        }

        const std::string &EnumBase::GetName(void) const
        {
            return name_;
        }
    }
}