#include "Argument.h"
#include "Type.h"
#include "Variant.h"

namespace cytx
{
    namespace meta
    {
        Argument::Argument(void)
            : typeID_(InvalidTypeID)
            , isArray_(false)
            , data_(nullptr) { }

        Argument::Argument(const Argument &rhs)
            : typeID_(rhs.typeID_)
            , isArray_(rhs.isArray_)
            , data_(rhs.data_) { }

        Argument::Argument(Variant &obj)
            : typeID_(obj.GetType().GetID())
            , isArray_(obj.GetType().IsArray())
            , data_(obj.getPtr()) { }

        Argument::Argument(const Variant &obj)
            : typeID_(obj.GetType().GetID())
            , isArray_(obj.GetType().IsArray())
            , data_(obj.getPtr()) { }

        Argument &Argument::operator=(const Argument &rhs)
        {
            data_ = rhs.data_;
            const_cast<TypeID&>(typeID_) = rhs.typeID_;
            return *this;
        }

        Type Argument::GetType(void) const
        {
            return Type(typeID_, isArray_);
        }

        void *Argument::GetPtr(void) const
        {
            return const_cast<void *>(data_);
        }

    }
}