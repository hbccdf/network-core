#pragma once
#include "MetaTraits.h"

namespace cytx
{
    namespace meta
    {
        class Variant;

        class DestructorInvokerBase
        {
        public:
            virtual ~DestructorInvokerBase(void) { }

            virtual void Invoke(const Variant& obj) = 0;
        };

        template<typename ClassType>
        class DestructorInvoker : public DestructorInvokerBase
        {
        public:
            void Invoke(const Variant &obj) override;
        };
    }
}


#include "Variant.h"
namespace cytx
{
    namespace meta
    {
        template<typename ClassType>
        void DestructorInvoker<ClassType>::Invoke(const Variant& obj)
        {
            auto &instance = obj.GetValue<ClassType>();

            instance.~ClassType();
        }
    }
}