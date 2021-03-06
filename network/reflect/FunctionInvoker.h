#pragma once
#include "MetaTraits.h"

namespace cytx
{
    namespace meta
    {
        class Variant;

        class FunctionInvokerBase
        {
        public:
            virtual ~FunctionInvokerBase() { }

            virtual Variant Invoke(const ArgumentList& arguments) = 0;
        };

        template<typename ReturnType, typename ...ArgTypes>
        class FunctionInvoker : public FunctionInvokerBase
        {
        public:
            typedef ReturnType (*Signature)(ArgTypes...);

            static_assert( sizeof...(ArgTypes) <= MaxArgumentCount,
                "Function has too many arguments. It's time to generate more overloads."
            );

            FunctionInvoker(Signature function);

            Variant Invoke(const ArgumentList& arguments) override;

        private:
            template<typename _>
            Variant invoke(const ArgumentList& arguments);

            template<typename _, typename A1>
            Variant invoke(const ArgumentList& arguments);

            template<typename _, typename A1, typename A2>
            Variant invoke(const ArgumentList& arguments);

            template<typename _, typename A1, typename A2, typename A3>
            Variant invoke(const ArgumentList& arguments);

            template<typename _, typename A1, typename A2, typename A3, typename A4>
            Variant invoke(const ArgumentList& arguments);

            template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5>
            Variant invoke(const ArgumentList& arguments);

            template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
            Variant invoke(const ArgumentList& arguments);

            template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
            Variant invoke(const ArgumentList& arguments);

            template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
            Variant invoke(const ArgumentList& arguments);

            template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
            Variant invoke(const ArgumentList& arguments);

            template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
            Variant invoke(const ArgumentList& arguments);

            Signature function_;
        };
    }
}


namespace cytx
{
    namespace meta
    {
        template<typename ReturnType, typename ... ArgTypes>
        FunctionInvoker<ReturnType, ArgTypes...>::FunctionInvoker(Signature function)
            : function_(function) { }

        template<typename ReturnType, typename ... ArgTypes>
        Variant FunctionInvoker<ReturnType, ArgTypes...>::Invoke(const ArgumentList &arguments)
        {
            /*UAssert(arguments.size() == THIS_ARG_COUNT,
                "Invalid function arguments.\nExpected %i args but got %i.",
                THIS_ARG_COUNT,
                arguments.size()
            );*/

            return invoke<void, ArgTypes...>(arguments);
        }

        template<typename ReturnType, typename ... ArgTypes>
        template<typename _>
        Variant FunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
        {
            return function_();
        }

        template<typename ReturnType, typename ... ArgTypes>
        template<typename _, typename A1>
        Variant FunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
        {
            return function_(
                arguments[0].GetValue<A1>()
            );
        }

        template<typename ReturnType, typename ... ArgTypes>
        template<typename _, typename A1, typename A2>
        Variant FunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
        {
            return function_(
                arguments[0].GetValue<A1>(),
                arguments[1].GetValue<A2>()
            );
        }

        template<typename ReturnType, typename ... ArgTypes>
        template<typename _, typename A1, typename A2, typename A3>
        Variant FunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
        {
            return function_(
                arguments[0].GetValue<A1>(),
                arguments[1].GetValue<A2>(),
                arguments[2].GetValue<A3>()
            );
        }

        template<typename ReturnType, typename ... ArgTypes>
        template<typename _, typename A1, typename A2, typename A3, typename A4>
        Variant FunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
        {
            return function_(
                arguments[0].GetValue<A1>(),
                arguments[1].GetValue<A2>(),
                arguments[2].GetValue<A3>(),
                arguments[3].GetValue<A4>()
            );
        }

        template<typename ReturnType, typename ... ArgTypes>
        template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5>
        Variant FunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
        {
            return function_(
                arguments[0].GetValue<A1>(),
                arguments[1].GetValue<A2>(),
                arguments[2].GetValue<A3>(),
                arguments[3].GetValue<A4>(),
                arguments[4].GetValue<A5>()
            );
        }

        template<typename ReturnType, typename ... ArgTypes>
        template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        Variant FunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
        {
            return function_(
                arguments[0].GetValue<A1>(),
                arguments[1].GetValue<A2>(),
                arguments[2].GetValue<A3>(),
                arguments[3].GetValue<A4>(),
                arguments[4].GetValue<A5>(),
                arguments[5].GetValue<A6>()
            );
        }

        template<typename ReturnType, typename ... ArgTypes>
        template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
        Variant FunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
        {
            return function_(
                arguments[0].GetValue<A1>(),
                arguments[1].GetValue<A2>(),
                arguments[2].GetValue<A3>(),
                arguments[3].GetValue<A4>(),
                arguments[4].GetValue<A5>(),
                arguments[5].GetValue<A6>(),
                arguments[6].GetValue<A7>()
            );
        }

        template<typename ReturnType, typename ... ArgTypes>
        template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
        Variant FunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
        {
            return function_(
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

        template<typename ReturnType, typename ... ArgTypes>
        template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        Variant FunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
        {
            return function_(
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

        template<typename ReturnType, typename ... ArgTypes>
        template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
        Variant FunctionInvoker<ReturnType, ArgTypes...>::invoke(const ArgumentList &arguments)
        {
            return function_(
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



        template<typename ...ArgTypes>
        class FunctionInvoker<void, ArgTypes...> : public FunctionInvokerBase
        {
        public:
            typedef void(*Signature)(ArgTypes...);

            static_assert(sizeof...(ArgTypes) <= MaxArgumentCount,
                "Function has too many arguments. It's time to generate more overloads."
                );

            FunctionInvoker(Signature function)
                : function_(function) { }

            Variant Invoke(const ArgumentList &arguments) override
            {
                /*UAssert(arguments.size() == THIS_ARG_COUNT,
                    "Invalid function arguments.\nExpected %i args but got %i.",
                    THIS_ARG_COUNT,
                    arguments.size()
                );*/

                invoke<void, ArgTypes...>(arguments);

                return{};
            }

        private:
            template<typename _>
            void invoke(const ArgumentList &arguments)
            {
                function_();
            }

            template<typename _, typename A1>
            void invoke(const ArgumentList &arguments)
            {
                function_(
                    arguments[0].GetValue<A1>()
                );
            }

            template<typename _, typename A1, typename A2>
            void invoke(const ArgumentList &arguments)
            {
                function_(
                    arguments[0].GetValue<A1>(),
                    arguments[1].GetValue<A2>()
                );
            }

            template<typename _, typename A1, typename A2, typename A3>
            void invoke(const ArgumentList &arguments)
            {
                function_(
                    arguments[0].GetValue<A1>(),
                    arguments[1].GetValue<A2>(),
                    arguments[2].GetValue<A3>()
                );
            }

            template<typename _, typename A1, typename A2, typename A3, typename A4>
            void invoke(const ArgumentList &arguments)
            {
                function_(
                    arguments[0].GetValue<A1>(),
                    arguments[1].GetValue<A2>(),
                    arguments[2].GetValue<A3>(),
                    arguments[3].GetValue<A4>()
                );
            }

            template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5>
            void invoke(const ArgumentList &arguments)
            {
                function_(
                    arguments[0].GetValue<A1>(),
                    arguments[1].GetValue<A2>(),
                    arguments[2].GetValue<A3>(),
                    arguments[3].GetValue<A4>(),
                    arguments[4].GetValue<A5>()
                );
            }

            template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
            void invoke(const ArgumentList &arguments)
            {
                function_(
                    arguments[0].GetValue<A1>(),
                    arguments[1].GetValue<A2>(),
                    arguments[2].GetValue<A3>(),
                    arguments[3].GetValue<A4>(),
                    arguments[4].GetValue<A5>(),
                    arguments[5].GetValue<A6>()
                );
            }

            template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
            void invoke(const ArgumentList &arguments)
            {
                function_(
                    arguments[0].GetValue<A1>(),
                    arguments[1].GetValue<A2>(),
                    arguments[2].GetValue<A3>(),
                    arguments[3].GetValue<A4>(),
                    arguments[4].GetValue<A5>(),
                    arguments[5].GetValue<A6>(),
                    arguments[6].GetValue<A7>()
                );
            }

            template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
            void invoke(const ArgumentList &arguments)
            {
                function_(
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

            template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
            void invoke(const ArgumentList &arguments)
            {
                function_(
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

            template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
            void invoke(const ArgumentList &arguments)
            {
                function_(
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

            Signature function_;
        };
    }
}