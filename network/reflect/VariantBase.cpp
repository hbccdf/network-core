#include "VariantBase.h"
#include "ArrayWrapper.h"

namespace cytx
{
    namespace meta
    {
        ArrayWrapper VariantBase::GetArray() const
        {
            // invalid wrapper
            return{};
        }
    }
}