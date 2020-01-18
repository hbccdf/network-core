#pragma once
#include <memory>
#include "MetaContainer.h"
#include "Variant.h"

namespace cytx
{
    namespace meta
    {
        class Variant;

        class FieldGetterBase
        {
        public:
            virtual ~FieldGetterBase() { }

            virtual Variant GetValue(const Variant& obj) = 0;
            virtual Variant GetValueReference(const Variant& obj) = 0;
        };

        class FieldSetterBase
        {
        public:
            virtual ~FieldSetterBase(void) { }

            virtual void SetValue(Variant& obj, const Variant& value) = 0;
        };


        template<typename ClassType, typename FieldType, bool IsMethod>
        class FieldGetter : public FieldGetterBase
        {
        public:
            Variant GetValue(const Variant& obj) override;
            Variant GetValueReference(const Variant& obj) override;
        };

        template<typename ClassType, typename FieldType, bool IsMethod>
        class FieldSetter : public FieldSetterBase
        {
        public:
            void SetValue(Variant& obj, const Variant& value) override;
        };

        class Field : public MetaContainer
        {
        public:
            static bool SetValue(Variant& instance, const Variant& value, const Method& setter);

            Field();

            Field(
                const std::string &name,
                Type type,
                Type classType,
                FieldGetterBase* getter,
                FieldSetterBase* setter
            );

            static const Field &Invalid();

            bool IsValid() const;
            bool IsReadOnly() const;

            Type GetType() const;
            Type GetClassType() const;

            const std::string& GetName() const;

            Variant GetValue(const Variant& instance) const;
            Variant GetValueReference(const Variant& instance) const;

            bool SetValue(Variant& instance, const Variant& value) const;

        private:
            friend struct TypeData;

            Type type_;
            Type classType_;

            std::string name_;

            std::shared_ptr<FieldGetterBase> getter_;
            std::shared_ptr<FieldSetterBase> setter_;
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

        template<typename ClassType, typename ReturnType>
        class FieldGetter<ClassType, ReturnType, true> : public FieldGetterBase
        {
        public:
            typedef ReturnType(ClassType::*Signature)(void);
            typedef ReturnType(ClassType::*SignatureConst)(void) const;

            FieldGetter(Signature method)
                : method_(reinterpret_cast<SignatureConst>(method)) { }

            FieldGetter(SignatureConst method)
                : method_(method) { }

            Variant GetValue(const Variant &obj) override
            {
                auto &instance = obj.GetValue<ClassType>();

                return (instance.*method_)();
            }

            Variant GetValueReference(const Variant &obj) override
            {
                return getValueReference(obj);
            }

        private:
            template<typename T = ReturnType>
            Variant getValueReference(
                const Variant &obj,
                typename std::enable_if<
                std::is_lvalue_reference<T>::value
                >::type* = nullptr
            )
            {
                auto &instance = obj.GetValue<ClassType>();

                return Variant{ (instance.*method_)(), variant_policy::NoCopy() };
            }

            template<typename T = ReturnType>
            Variant getValueReference(
                const Variant &obj,
                typename std::enable_if<
                !std::is_lvalue_reference<T>::value
                >::type* = nullptr
            )
            {
                // non l-value reference return types must return by value
                return GetValue(obj);
            }

            SignatureConst method_;
        };

        ///////////////////////////////////////////////////////////////////////
        // Getter from Direct Field
        ///////////////////////////////////////////////////////////////////////

        template<typename ClassType, typename FieldType>
        class FieldGetter<ClassType, FieldType, false> : public FieldGetterBase
        {
        public:
            typedef FieldType(ClassType::*Signature);

            FieldGetter(Signature field)
                : field_(field) { }

            Variant GetValue(const Variant &obj) override
            {
                auto &instance = obj.GetValue<ClassType>();

                return instance.*field_;
            }

            Variant GetValueReference(const Variant &obj) override
            {
                auto &instance = obj.GetValue<ClassType>();

                return Variant{ instance.*field_, variant_policy::NoCopy() };
            }

        private:
            Signature field_;
        };



        ///////////////////////////////////////////////////////////////////////
        // Setter from Method
        ///////////////////////////////////////////////////////////////////////

        template<typename ClassType, typename ArgumentType>
        class FieldSetter<ClassType, ArgumentType, true> : public FieldSetterBase
        {
        public:
            typedef typename std::remove_reference<ArgumentType>::type NonReferenceArgType;
            typedef void (ClassType::*Signature)(ArgumentType);
            typedef void (ClassType::*SignatureConst)(ArgumentType) const;

            FieldSetter(Signature method)
                : method_(method) { }

            FieldSetter(SignatureConst method)
                : method_(reinterpret_cast<Signature>(method)) { }

            void SetValue(Variant &obj, const Variant &value) override
            {
                UAssert(value.IsValid(), "Setting invalid value.");

                auto &instance = obj.GetValue<ClassType>();

                (instance.*method_)(value.GetValue<NonReferenceArgType>());
            }

        private:
            Signature method_;
        };

        ///////////////////////////////////////////////////////////////////////
        // Setter from Direct Field
        ///////////////////////////////////////////////////////////////////////

        template<typename ClassType, typename FieldType>
        class FieldSetter<ClassType, FieldType, false> : public FieldSetterBase
        {
        public:
            typedef FieldType(ClassType::*Signature);
            typedef typename std::remove_const<FieldType>::type NonConstFieldType;

            FieldSetter(Signature field)
                : field_(field) { }

            void SetValue(Variant &obj, const Variant &value) override
            {
                //UAssert(value.IsValid(), "Setting invalid value.");

                auto &instance = obj.GetValue<ClassType>();

                const_cast<NonConstFieldType&>(instance.*field_) = value.GetValue<FieldType>();
            }

        private:
            Signature field_;
        };
    }
}