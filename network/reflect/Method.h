#pragma once
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include "MetaContainer.h"
#include "Invokable.h"
#include "Variant.h"
#include "Argument.h"
#include "MethodInvoker.h"

namespace cytx
{
    namespace meta
    {
        class Method : public MetaContainer, public Invokable
        {
        public:
            Method();

            template<class ClassType, typename ReturnType, typename ...ArgTypes>
            Method(
                const std::string& name,
                ReturnType(ClassType::*method)(ArgTypes...)
            )
                : Invokable(name)
                , isConst_(false)
                , classType_(typeof(ClassType))
                , invoker_(new MethodInvoker<ClassType, ReturnType, ArgTypes...>(method))
            {
                TypeUnpacker<ArgTypes...>::Apply(signature_);
            }

            // detect const-ness
            template<class ClassType, typename ReturnType, typename ...ArgTypes>
            Method(
                const std::string& name,
                ReturnType(ClassType::*method)(ArgTypes...) const
            )
                : Invokable(name)
                , isConst_(true)
                , classType_(typeof(ClassType))
                , invoker_(new MethodInvoker<ClassType, ReturnType, ArgTypes...>(method))
            {
                TypeUnpacker<ArgTypes...>::Apply(signature_);
            }

            static const Method &Invalid();

            Type GetClassType() const;

            bool IsValid() const;
            bool IsConst() const;

            Variant Invoke(Variant& instance, ArgumentList& arguments) const;

            template<typename ...Args>
            Variant Invoke(Variant& instance, Args &&...args) const
            {
                ArgumentList arguments{ std::forward<Args>(args)... };
                return Invoke(instance, arguments);
            }

        private:
            bool isConst_;
            Type classType_;
            std::shared_ptr<MethodInvokerBase> invoker_;
        };
    }
}