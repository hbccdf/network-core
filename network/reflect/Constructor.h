#pragma once
#include <memory>
#include "MetaContainer.h"
#include "Invokable.h"
#include "Type.h"
#include "ConstructorInvoker.h"

namespace cytx
{
    namespace meta
    {
        class Variant;
        class Argument;

        class Constructor : public MetaContainer, public Invokable
        {
        public:
            Constructor();
            Constructor(const Constructor& rhs);
            Constructor(const Constructor&& rhs) noexcept;

            Constructor(
                Type classType,
                InvokableSignature signature,
                ConstructorInvokerBase* invoker,
                bool isDynamic
            );

            Constructor& operator=(const Constructor&& rhs);

            static const Constructor& Invalid();

            Type GetClassType() const;

            bool IsValid() const;
            bool IsDynamic() const;

            Variant InvokeVariadic(const ArgumentList& arguments) const;

            template<typename ... Args>
            Variant Invoke(Args&& ... args) const;

        private:
            bool isDynamic_;

            Type classType_;

            std::shared_ptr<ConstructorInvokerBase> invoker_;
        };
    }
}


#include "Variant.h"
#include "Argument.h"

namespace cytx
{
    namespace meta
    {
        template<typename ...Args>
        Variant Constructor::Invoke(Args &&...args) const
        {
            ArgumentList arguments{ std::forward<Args>(args)... };

            return InvokeVariadic(arguments);
        }
    }
}