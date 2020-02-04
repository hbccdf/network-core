#include "ArrayWrapper.h"

namespace cytx
{
    namespace meta
    {
        ArrayWrapper::ArrayWrapper(void)
            : isConst_(true)
            , base_(nullptr) { }

        Variant ArrayWrapper::GetValue(size_t index) const
        {
            return base_ ? base_->GetValue(index) : Variant();
        }

        void ArrayWrapper::SetValue(size_t index, const Argument &value)
        {
            //UAssert(!m_isConst, "Array is const.");

            if (base_)
                base_->SetValue(index, value);
        }

        void ArrayWrapper::Insert(size_t index, const Argument &value)
        {
            //UAssert(!m_isConst, "Array is const.");

            if (base_)
                base_->Insert(index, value);
        }

        void ArrayWrapper::Remove(size_t index)
        {
            //UAssert(!m_isConst, "Array is const.");

            if (base_)
                base_->Remove(index);
        }

        size_t ArrayWrapper::Size(void) const
        {
            return base_ ? base_->Size() : 0;
        }

        bool ArrayWrapper::IsValid(void) const
        {
            return base_ != nullptr;
        }

        bool ArrayWrapper::IsConst(void) const
        {
            return isConst_;
        }
    }
}