#pragma once
#include "MetaTraits.h"

namespace cytx
{
    namespace meta
    {
        class Variant;

        class MethodInvokerBase
        {
        public:
            virtual ~MethodInvokerBase() { }

            virtual Variant Invoke(Variant& obj, const ArgumentList& arguments) = 0;
        };

        template<typename ClassType, typename ReturnType, typename ...ArgTypes>
        class MethodInvoker : public MethodInvokerBase
        {
        public:
            typedef ReturnType (ClassType::*Signature)(ArgTypes...);
            typedef ReturnType (ClassType::*ConstSignature)(ArgTypes...) const;

            static_assert( sizeof...(ArgTypes) <= MaxArgumentCount,
                "Method has too many arguments. It's time to generate more overloads."
            );

            MethodInvoker(Signature method);
            MethodInvoker(ConstSignature method);

            Variant Invoke(Variant& obj, const ArgumentList& arguments) override;

        private:
            template<typename _>
            Variant invoke(Variant& obj, const ArgumentList& arguments);

            template<typename _, typename A1>
            Variant invoke(Variant& obj, const ArgumentList& arguments);

            template<typename _, typename A1, typename A2>
            Variant invoke(Variant& obj, const ArgumentList& arguments);

            template<typename _, typename A1, typename A2, typename A3>
            Variant invoke(Variant& obj, const ArgumentList& arguments);

            template<typename _, typename A1, typename A2, typename A3, typename A4>
            Variant invoke(Variant& obj, const ArgumentList& arguments);

            template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5>
            Variant invoke(Variant& obj, const ArgumentList& arguments);

            template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
            Variant invoke(Variant& obj, const ArgumentList& arguments);

            template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
            Variant invoke(Variant& obj, const ArgumentList& arguments);

            template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
            Variant invoke(Variant& obj, const ArgumentList& arguments);

            template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
            Variant invoke(Variant& obj, const ArgumentList& arguments);

            template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
            Variant invoke(Variant& obj, const ArgumentList& arguments);

            ConstSignature method_;
        };
    }
}



namespace cytx
{
    namespace meta
    {
        template<typename ClassType, typename ReturnType, typename ... ArgTypes>
        MethodInvoker<ClassType, ReturnType, ArgTypes...>::MethodInvoker(Signature method)
            : method_(reinterpret_cast<ConstSignature>(method)) { }

        template<typename ClassType, typename ReturnType, typename ... ArgTypes>
        MethodInvoker<ClassType, ReturnType, ArgTypes...>::MethodInvoker(ConstSignature method)
            : method_(method) { }

        ///////////////////////////////////////////////////////////////////////

        template<typename ClassType, typename ReturnType, typename ... ArgTypes>
        Variant MethodInvoker<ClassType, ReturnType, ArgTypes...>::Invoke(Variant &obj, const ArgumentList &arguments)
        {
            /*UAssert(arguments.size() == THIS_ARG_COUNT,
                "Invalid method arguments.\nExpected %i args but got %i.",
                THIS_ARG_COUNT,
                arguments.size()
            );*/

            return invoke<void, ArgTypes...>(obj, arguments);
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename ClassType, typename ReturnType, typename ... ArgTypes>
        template<typename _>
        Variant MethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(Variant &obj, const ArgumentList &arguments)
        {
            auto &instance = obj.GetValue<ClassType>();

            return (instance.*method_)();
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename ClassType, typename ReturnType, typename ... ArgTypes>
        template<typename _, typename A1>
        Variant MethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(Variant &obj, const ArgumentList &arguments)
        {
            auto &instance = obj.GetValue<ClassType>();

            return (instance.*method_)(
                arguments[0].GetValue<A1>()
                );
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename ClassType, typename ReturnType, typename ... ArgTypes>
        template<typename _, typename A1, typename A2>
        Variant MethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(Variant &obj, const ArgumentList &arguments)
        {
            auto &instance = obj.GetValue<ClassType>();

            return (instance.*method_)(
                arguments[0].GetValue<A1>(),
                arguments[1].GetValue<A2>()
                );
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename ClassType, typename ReturnType, typename ... ArgTypes>
        template<typename _, typename A1, typename A2, typename A3>
        Variant MethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(Variant &obj, const ArgumentList &arguments)
        {
            auto &instance = obj.GetValue<ClassType>();

            return (instance.*method_)(
                arguments[0].GetValue<A1>(),
                arguments[1].GetValue<A2>(),
                arguments[2].GetValue<A3>()
                );
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename ClassType, typename ReturnType, typename ... ArgTypes>
        template<typename _, typename A1, typename A2, typename A3, typename A4>
        Variant MethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(Variant &obj, const ArgumentList &arguments)
        {
            auto &instance = obj.GetValue<ClassType>();

            return (instance.*method_)(
                arguments[0].GetValue<A1>(),
                arguments[1].GetValue<A2>(),
                arguments[2].GetValue<A3>(),
                arguments[3].GetValue<A4>()
                );
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename ClassType, typename ReturnType, typename ... ArgTypes>
        template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5>
        Variant MethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(Variant &obj, const ArgumentList &arguments)
        {
            auto &instance = obj.GetValue<ClassType>();

            return (instance.*method_)(
                arguments[0].GetValue<A1>(),
                arguments[1].GetValue<A2>(),
                arguments[2].GetValue<A3>(),
                arguments[3].GetValue<A4>(),
                arguments[4].GetValue<A5>()
                );
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename ClassType, typename ReturnType, typename ... ArgTypes>
        template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        Variant MethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(Variant &obj, const ArgumentList &arguments)
        {
            auto &instance = obj.GetValue<ClassType>();

            return (instance.*method_)(
                arguments[0].GetValue<A1>(),
                arguments[1].GetValue<A2>(),
                arguments[2].GetValue<A3>(),
                arguments[3].GetValue<A4>(),
                arguments[4].GetValue<A5>(),
                arguments[5].GetValue<A6>()
                );
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename ClassType, typename ReturnType, typename ... ArgTypes>
        template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
        Variant MethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(Variant &obj, const ArgumentList &arguments)
        {
            auto &instance = obj.GetValue<ClassType>();

            return (instance.*method_)(
                arguments[0].GetValue<A1>(),
                arguments[1].GetValue<A2>(),
                arguments[2].GetValue<A3>(),
                arguments[3].GetValue<A4>(),
                arguments[4].GetValue<A5>(),
                arguments[5].GetValue<A6>(),
                arguments[6].GetValue<A7>()
                );
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename ClassType, typename ReturnType, typename ... ArgTypes>
        template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
        Variant MethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(Variant &obj, const ArgumentList &arguments)
        {
            auto &instance = obj.GetValue<ClassType>();

            return (instance.*method_)(
                arguments[0].GetValue<A1>(),
                arguments[1].GetValue<A2>(),
                arguments[2].GetValue<A3>(),
                arguments[3].GetValue<A4>(),
                arguments[4].GetValue<A5>(),
                arguments[5].GetValue<A6>(),
                arguments[6].GetValue<A7>(),
                arguments[7].GetValue<A8>()
                );
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename ClassType, typename ReturnType, typename ... ArgTypes>
        template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        Variant MethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(Variant &obj, const ArgumentList &arguments)
        {
            auto &instance = obj.GetValue<ClassType>();

            return (instance.*method_)(
                arguments[0].GetValue<A1>(),
                arguments[1].GetValue<A2>(),
                arguments[2].GetValue<A3>(),
                arguments[3].GetValue<A4>(),
                arguments[4].GetValue<A5>(),
                arguments[5].GetValue<A6>(),
                arguments[6].GetValue<A7>(),
                arguments[7].GetValue<A8>(),
                arguments[8].GetValue<A9>()
                );
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename ClassType, typename ReturnType, typename ... ArgTypes>
        template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
        Variant MethodInvoker<ClassType, ReturnType, ArgTypes...>::invoke(Variant &obj, const ArgumentList &arguments)
        {
            auto &instance = obj.GetValue<ClassType>();

            return (instance.*method_)(
                arguments[0].GetValue<A1>(),
                arguments[1].GetValue<A2>(),
                arguments[2].GetValue<A3>(),
                arguments[3].GetValue<A4>(),
                arguments[4].GetValue<A5>(),
                arguments[5].GetValue<A6>(),
                arguments[6].GetValue<A7>(),
                arguments[7].GetValue<A8>(),
                arguments[8].GetValue<A9>(),
                arguments[9].GetValue<A10>()
                );
        }




        template<typename ClassType, typename ...ArgTypes>
        class MethodInvoker<ClassType, void, ArgTypes...> : public MethodInvokerBase
        {
        public:
            typedef void (ClassType::*Signature)(ArgTypes...);
            typedef void (ClassType::*ConstSignature)(ArgTypes...) const;

            static_assert(sizeof...(ArgTypes) <= MaxArgumentCount,
                "Method has too many arguments. It's time to generate more overloads."
                );

            MethodInvoker(Signature method)
                : method_(reinterpret_cast<ConstSignature>(method)) { }

            MethodInvoker(ConstSignature method)
                : method_(method) { }

            Variant Invoke(Variant &obj, const ArgumentList &arguments) override
            {
                /*UAssert(arguments.size() == THIS_ARG_COUNT,
                    "Invalid method arguments.\nExpected %i args but got %i.",
                    THIS_ARG_COUNT,
                    arguments.size()
                );*/

                invoke<void, ArgTypes...>(obj, arguments);

                return{};
            }

        private:
            template<typename X>
            void invoke(Variant &obj, const ArgumentList &arguments)
            {
                auto &instance = obj.GetValue<ClassType>();

                (instance.*method_)();
            }

            template<typename X, typename A1>
            void invoke(Variant &obj, const ArgumentList &arguments)
            {
                auto &instance = obj.GetValue<ClassType>();

                (instance.*method_)(
                    arguments[0].GetValue<A1>()
                    );
            }

            template<typename X, typename A1, typename A2>
            void invoke(Variant &obj, const ArgumentList &arguments)
            {
                auto &instance = obj.GetValue<ClassType>();

                (instance.*method_)(
                    arguments[0].GetValue<A1>(),
                    arguments[1].GetValue<A2>()
                    );
            }

            template<typename X, typename A1, typename A2, typename A3>
            void invoke(Variant &obj, const ArgumentList &arguments)
            {
                auto &instance = obj.GetValue<ClassType>();

                (instance.*method_)(
                    arguments[0].GetValue<A1>(),
                    arguments[1].GetValue<A2>(),
                    arguments[2].GetValue<A3>()
                    );
            }

            template<typename X, typename A1, typename A2, typename A3, typename A4>
            void invoke(Variant &obj, const ArgumentList &arguments)
            {
                auto &instance = obj.GetValue<ClassType>();

                (instance.*method_)(
                    arguments[0].GetValue<A1>(),
                    arguments[1].GetValue<A2>(),
                    arguments[2].GetValue<A3>(),
                    arguments[3].GetValue<A4>()
                    );
            }

            template<typename X, typename A1, typename A2, typename A3, typename A4, typename A5>
            void invoke(Variant &obj, const ArgumentList &arguments)
            {
                auto &instance = obj.GetValue<ClassType>();

                (instance.*method_)(
                    arguments[0].GetValue<A1>(),
                    arguments[1].GetValue<A2>(),
                    arguments[2].GetValue<A3>(),
                    arguments[3].GetValue<A4>(),
                    arguments[4].GetValue<A5>()
                    );
            }

            template<typename X, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
            void invoke(Variant &obj, const ArgumentList &arguments)
            {
                auto &instance = obj.GetValue<ClassType>();

                (instance.*method_)(
                    arguments[0].GetValue<A1>(),
                    arguments[1].GetValue<A2>(),
                    arguments[2].GetValue<A3>(),
                    arguments[3].GetValue<A4>(),
                    arguments[4].GetValue<A5>(),
                    arguments[5].GetValue<A6>()
                    );
            }

            template<typename X, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
            void invoke(Variant &obj, const ArgumentList &arguments)
            {
                auto &instance = obj.GetValue<ClassType>();

                (instance.*method_)(
                    arguments[0].GetValue<A1>(),
                    arguments[1].GetValue<A2>(),
                    arguments[2].GetValue<A3>(),
                    arguments[3].GetValue<A4>(),
                    arguments[4].GetValue<A5>(),
                    arguments[5].GetValue<A6>(),
                    arguments[6].GetValue<A7>()
                    );
            }

            template<typename X, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
            void invoke(Variant &obj, const ArgumentList &arguments)
            {
                auto &instance = obj.GetValue<ClassType>();

                (instance.*method_)(
                    arguments[0].GetValue<A1>(),
                    arguments[1].GetValue<A2>(),
                    arguments[2].GetValue<A3>(),
                    arguments[3].GetValue<A4>(),
                    arguments[4].GetValue<A5>(),
                    arguments[5].GetValue<A6>(),
                    arguments[6].GetValue<A7>(),
                    arguments[7].GetValue<A8>()
                    );
            }

            template<typename X, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
            void invoke(Variant &obj, const ArgumentList &arguments)
            {
                auto &instance = obj.GetValue<ClassType>();

                (instance.*method_)(
                    arguments[0].GetValue<A1>(),
                    arguments[1].GetValue<A2>(),
                    arguments[2].GetValue<A3>(),
                    arguments[3].GetValue<A4>(),
                    arguments[4].GetValue<A5>(),
                    arguments[5].GetValue<A6>(),
                    arguments[6].GetValue<A7>(),
                    arguments[7].GetValue<A8>(),
                    arguments[8].GetValue<A9>()
                    );
            }

            template<typename X, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
            void invoke(Variant &obj, const ArgumentList &arguments)
            {
                auto &instance = obj.GetValue<ClassType>();

                (instance.*method_)(
                    arguments[0].GetValue<A1>(),
                    arguments[1].GetValue<A2>(),
                    arguments[2].GetValue<A3>(),
                    arguments[3].GetValue<A4>(),
                    arguments[4].GetValue<A5>(),
                    arguments[5].GetValue<A6>(),
                    arguments[6].GetValue<A7>(),
                    arguments[7].GetValue<A8>(),
                    arguments[8].GetValue<A9>(),
                    arguments[9].GetValue<A10>()
                    );
            }

            ConstSignature method_;
        };
    }
}