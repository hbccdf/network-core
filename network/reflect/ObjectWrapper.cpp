#include "ObjectWrapper.h"
#include "Object.h"

#include "Type.h"

namespace cytx
{
    namespace meta
    {
        ObjectWrapper::ObjectWrapper(Object *instance)
            : object_( instance )
        {
        }

        Type ObjectWrapper::GetType(void) const
        {
            return object_->GetType( );
        }

        void *ObjectWrapper::GetPtr(void) const
        {
            return object_;
        }

        int ObjectWrapper::ToInt(void) const
        {
            return int( );
        }

        bool ObjectWrapper::ToBool(void) const
        {
            return bool( );
        }

        float ObjectWrapper::ToFloat(void) const
        {
            return float( );
        }

        double ObjectWrapper::ToDouble(void) const
        {
            return double( );
        }

        std::string ObjectWrapper::ToString(void) const
        {
            return std::string( );
        }

        VariantBase *ObjectWrapper::Clone(void) const
        {
            return new ObjectWrapper(object_);
        }
    }
}