#pragma once
#include "VariantBase.h"
#include "Array.h"
#include "TypeID.h"
#include "Object.h"

#define DISABLE_VARIANT typename std::enable_if<!std::is_same<Variant, T>::value>::type*
#define DISABLE_VARIANT_DECL DISABLE_VARIANT = nullptr

#define DISABLE_ARGUMENT typename std::enable_if<!std::is_same<Argument, T>::value>::type*
#define DISABLE_ARGUMENT_DECL DISABLE_ARGUMENT = nullptr

#define DISABLE_CONST typename std::enable_if<!std::is_const<T>::value>::type*
#define DISABLE_CONST_DECL DISABLE_CONST = nullptr

#define ENABLE_IS_OBJECT typename std::enable_if<std::is_base_of<Object, T>::value>::type* = nullptr

namespace cytx
{
    namespace meta
    {
        class Argument;
        class ArrayWrapper;

        class Variant
        {
        public:
            Variant();

            // Given a type that inherits from a meta::Object and
            // supplied with the "WrapObject" policy, use the object wrapper
            // variant base
            template<typename T>
            Variant(T* data, variant_policy::WrapObject, ENABLE_IS_OBJECT);

            template<typename T>
            Variant(T& data, DISABLE_VARIANT_DECL);

            template<typename T>
            Variant(T& data, variant_policy::NoCopy, DISABLE_VARIANT_DECL);

            // non-const r-value references, excluding other variants and arguments
            template<typename T>
            Variant(T&& data, DISABLE_VARIANT_DECL, DISABLE_ARGUMENT_DECL, DISABLE_CONST_DECL);

            template<typename T>
            Variant(Array<T>& rhs);

            template<typename T>
            Variant(const Array<T>& rhs);

            template<typename T>
            Variant(Array<T>&& rhs);

            template<typename T>
            Variant(const Array<T>&& rhs);

            Variant(const Variant& rhs);

            Variant(Variant&& rhs);

            ~Variant();

            template<typename T>
            Variant& operator=(T&& rhs);

            Variant& operator=(const Variant& rhs);

            Variant& operator=(Variant&& rhs);

            operator bool() const;

            Type GetType() const;
            ArrayWrapper GetArray() const;

            void Swap(Variant& other);

            int ToInt() const;
            bool ToBool() const;
            float ToFloat() const;
            double ToDouble() const;
            std::string ToString() const;

            template<typename T>
            T& GetValue(void) const;

            bool IsValid(void) const;
            bool IsConst(void) const;
            bool IsArray(void) const;

        private:
            friend class Type;
            friend class Argument;
            friend class Destructor;

            void *getPtr(void) const;

            bool isConst_;

            VariantBase* base_;
        };
    }
}


#include "VariantContainer.h"
#include "ObjectWrapper.h"
#include "ArrayVariantContainer.h"

namespace cytx
{
    namespace meta
    {
        template <typename T>
        Variant::Variant(
            T *data,
            variant_policy::WrapObject,
            typename std::enable_if<
            std::is_base_of<Object, T>::value
            >::type*
        )
            : isConst_(std::is_const<T>::value)
            , base_(
                new ObjectWrapper(
                    static_cast<Object *>(
                        const_cast<typename std::remove_const<T>::type*>(
                            data
                            )
                        )
                )
            )
        {

        }

        ///////////////////////////////////////////////////////////////////////

        template<typename T>
        Variant::Variant(
            T &data,
            DISABLE_VARIANT
        )
            : isConst_(std::is_pointer<T>::value && std::is_const<T>::value)
            , base_(new VariantContainer< CleanedType<T> >(data))
        {

        }

        ///////////////////////////////////////////////////////////////////////

        template<typename T>
        Variant::Variant(
            T &data,
            variant_policy::NoCopy,
            DISABLE_VARIANT
        )
            : isConst_(std::is_pointer<T>::value && std::is_const<T>::value)
            , base_(new VariantContainer< CleanedType<T>& >(data))
        {

        }

        ///////////////////////////////////////////////////////////////////////

        template<typename T>
        Variant::Variant(
            T &&data,
            DISABLE_VARIANT,
            DISABLE_ARGUMENT,
            DISABLE_CONST
        )
            : isConst_(false)
            , base_(
                new VariantContainer< CleanedType<T> >(
                    static_cast<T&&>(data)
                    )
            )
        {

        }

        ///////////////////////////////////////////////////////////////////////

        template<typename T>
        Variant::Variant(Array<T> &rhs)
            : isConst_(false)
            , base_(new ArrayVariantContainer<T, ArrayByReference<T>>(rhs))
        {

        }

        ///////////////////////////////////////////////////////////////////////

        template<typename T>
        Variant::Variant(const Array<T> &rhs)
            : isConst_(true)
            , base_(new ArrayVariantContainer<T, const ArrayByReference<T>>(const_cast<const ArrayByReference<T>>(rhs)))
        {

        }

        ///////////////////////////////////////////////////////////////////////

        template<typename T>
        Variant::Variant(Array<T> &&rhs)
            : isConst_(false)
            , base_(new ArrayVariantContainer<T, ArrayByValue<T>>(rhs))
        {

        }

        ///////////////////////////////////////////////////////////////////////

        template<typename T>
        Variant::Variant(const Array<T> &&rhs)
            : isConst_(true)
            , base_(new ArrayVariantContainer<T, const ArrayByValue<T>>(rhs))
        {

        }

        ///////////////////////////////////////////////////////////////////////

        template<typename T>
        Variant &Variant::operator=(T &&rhs)
        {
            Variant(static_cast<T&&>(rhs)).Swap(*this);

            return *this;
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename T>
        T &Variant::GetValue(void) const
        {
            return *static_cast<T*>(getPtr());
        }
    }
}