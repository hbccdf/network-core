#pragma once
#include <memory>
#include "MetaTraits.h"
#include "MetaContainer.h"
#include "Variant.h"

namespace cytx
{
    namespace meta
    {
        class Variant;

        class GlobalGetterBase
        {
        public:
            virtual ~GlobalGetterBase() { }

            virtual Variant GetValue() = 0;
        };

        class GlobalSetterBase
        {
        public:
            virtual ~GlobalSetterBase() { }

            virtual void SetValue(const Argument& value) = 0;
        };

        template<typename GlobalType, bool IsMethod>
        class GlobalGetter : public GlobalGetterBase
        {
        public:
            Variant GetValue() override;
        };

        template<typename GlobalType, bool IsMethod>
        class GlobalSetter : public GlobalSetterBase
        {
        public:
            void SetValue(const Argument& value) override;
        };

        class Global : public MetaContainer
        {
        public:
            Global();

            Global(
                const std::string &name,
                Type type,
                GlobalGetterBase *getter,
                GlobalSetterBase *setter,
                Type parentType = Type::Invalid()
            );

            static const Global& Invalid();

            bool IsValid() const;
            bool IsReadOnly() const;

            Type GetType() const;
            Type GetParentType() const;

            const std::string& GetName() const;

            Variant GetValue() const;
            bool SetValue(const Argument& value) const;

        private:
            friend struct TypeData;

            Type type_;
            Type parentType_;

            std::string name_;

            std::shared_ptr<GlobalGetterBase> getter_;
            std::shared_ptr<GlobalSetterBase> setter_;
        };
    }
}


namespace cytx
{
    namespace meta
    {
        ///////////////////////////////////////////////////////////////////////
        // Getter from Method
        ///////////////////////////////////////////////////////////////////////

        template<typename GlobalType>
        class GlobalGetter<GlobalType, true> : public GlobalGetterBase
        {
        public:
            typedef GlobalType(*Signature)(void);

            GlobalGetter(Signature function)
                : function_(function) { }

            Variant GetValue(void) override
            {
                return function_();
            }

        private:
            Signature function_;
        };

        ///////////////////////////////////////////////////////////////////////
        // Getter from Direct Global
        ///////////////////////////////////////////////////////////////////////

        template<typename GlobalType>
        class GlobalGetter<GlobalType, false> : public GlobalGetterBase
        {
        public:
            GlobalGetter(GlobalType *global)
                : global_(global) { }

            Variant GetValue(void) override
            {
                return *global_;
            }

        private:
            GlobalType *global_;
        };




        ///////////////////////////////////////////////////////////////////////
        // Setter from Method
        ///////////////////////////////////////////////////////////////////////

        template<typename GlobalType>
        class GlobalSetter<GlobalType, true> : public GlobalSetterBase
        {
        public:
            typedef GlobalType(*Signature)(const GlobalType &);

            GlobalSetter(Signature function)
                : function_(function) { }

            void SetValue(const Argument &value) override
            {
                function_(value.GetValue<GlobalType>());
            }

        private:
            Signature function_;
        };

        ///////////////////////////////////////////////////////////////////////
        // Setter from Direct Field
        ///////////////////////////////////////////////////////////////////////

        template<typename GlobalType>
        class GlobalSetter<GlobalType, false> : public GlobalSetterBase
        {
        public:
            GlobalSetter(GlobalType *field)
                : field_(field) { }

            void SetValue(const Argument &value) override
            {
                const_cast<GlobalType&>(*field_) = value.GetValue<GlobalType>();
            }

        private:
            GlobalType *field_;
        };
    }
}