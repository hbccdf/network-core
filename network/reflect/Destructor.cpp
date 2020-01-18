#include "Destructor.h"
#include "Variant.h"

namespace cytx
{
    namespace meta
    {
        namespace
        {
            const auto cDestructorName = "destructor";
        }

        Destructor::Destructor(void)
            : Invokable()
            , classType_(Type::Invalid()) { }

        Destructor::Destructor(Type classType, DestructorInvokerBase *invoker)
            : Invokable(cDestructorName)
            , classType_(classType)
            , invoker_(invoker) { }

        Type Destructor::GetClassType(void) const
        {
            return classType_;
        }

        void Destructor::Invoke(Variant &instance) const
        {
            /*UAssert(IsValid(),
                "Invalid constructor invoked."
            );

            UAssert(m_classType == instance.GetType(),
                "Destructor called on incompatible type."
            );*/

            invoker_->Invoke(instance);

            delete instance.base_;

            instance.base_ = nullptr;
        }

        const Destructor &Destructor::Invalid(void)
        {
            static Destructor invalid;

            return invalid;
        }

        bool Destructor::IsValid(void) const
        {
            return invoker_ != nullptr;
        }
    }
}