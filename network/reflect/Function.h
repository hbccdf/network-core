#pragma once
#include <functional>
#include <memory>
#include "MetaContainer.h"
#include "Invokable.h"
#include "Variant.h"
#include "Argument.h"
#include "FunctionInvoker.h"

namespace cytx
{
    namespace meta
    {
        class Function : public MetaContainer , public Invokable
        {
        public:
            using Invoker = std::function<Variant(ArgumentList&)>;

            Function(void);

            template<typename ReturnType, typename ...ArgTypes>
            Function(
                const std::string &name,
                ReturnType (*function)(ArgTypes...),
                Type parentType = Type::Invalid( )
            )
                : Invokable(name)
                , parentType_(parentType)
                , invoker_(new FunctionInvoker<ReturnType, ArgTypes...>(function))
            {
                TypeUnpacker<ArgTypes...>::Apply(signature_);
            }

            static const Function &Invalid();

            Type GetParentType() const;

            bool IsValid() const;

            Variant InvokeVariadic(ArgumentList& arguments) const;

            template<typename ...Args>
            Variant Invoke(Args &&...args) const
            {
                ArgumentList arguments{ std::forward<Args>(args)... };
                return InvokeVariadic(arguments);
            }

        private:
            Type parentType_;

            std::shared_ptr<FunctionInvokerBase> invoker_;
        };
    }
}