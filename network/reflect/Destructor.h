#pragma once
#include <memory>
#include "Invokable.h"
#include "DestructorInvoker.h"

namespace cytx
{
    namespace meta
    {
        class Destructor : public Invokable
        {
        public:
            Destructor();
            Destructor(Type classType, DestructorInvokerBase* invoker);

            static const Destructor& Invalid();

            Type GetClassType() const;

            bool IsValid() const;

            void Invoke(Variant& instance) const;

        private:
            Type classType_;

            std::shared_ptr<DestructorInvokerBase> invoker_;
        };
    }
}