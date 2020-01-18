#pragma once
#include "MetaTraits.h"
#include "Object.h"
#include "VariantBase.h"

#define DEFAULT_TYPE_HANDLER(typeName)                                                          \
    template<typename U = T>                                                                    \
    typeName get##typeName(                                                                     \
        typename std::enable_if<                                                                \
            !std::is_convertible<typename TypeOrEnumType<U>::type, typeName>::value             \
        >::type* = nullptr                                                                      \
    ) const;                                                                                    \
    template<typename U = T>                                                                    \
    typeName get##typeName(                                                                     \
        typename std::enable_if<                                                                \
            std::is_convertible<typename TypeOrEnumType<U>::type, typeName>::value              \
        >::type* = nullptr                                                                      \
    ) const;

namespace cytx
{
    namespace meta
    {
        template<typename T>
        class VariantContainer : public VariantBase
        {
        public:
            using NonRefType = typename std::remove_reference<T>::type;

            VariantContainer(const NonRefType& value);
            VariantContainer(const NonRefType&& value);

            Type GetType() const override;

            void* GetPtr() const override;

            int ToInt() const override;
            bool ToBool() const override;
            float ToFloat() const override;
            double ToDouble() const override;
            std::string ToString() const override;

            VariantBase *Clone() const override;
        private:
            friend class Variant;

            T value_;

            VariantContainer& operator=(const VariantContainer& rhs) = delete;

            DEFAULT_TYPE_HANDLER(int);
            DEFAULT_TYPE_HANDLER(bool);
            DEFAULT_TYPE_HANDLER(float);
            DEFAULT_TYPE_HANDLER(double);

            template<typename U = T>
            std::string getString(typename std::enable_if<!std::is_arithmetic<U>::value>::type* = nullptr) const;

            template<typename U = T>
            std::string getString(typename std::enable_if<std::is_arithmetic<U>::value>::type* = nullptr) const;
        };
    }
}



#include "Type.h"
#include "Compiler.h"

#if defined(COMPILER_MSVC)
#pragma warning(push)
// unused template parameters
#pragma warning(disable : 4544)
// implicit conversion performance warnings
#pragma warning(disable : 4800)
// possible loss of data
#pragma warning(disable : 4244)
#endif

#define DEFAULT_TYPE_HANDLER_IMPL(typeName)                                                     \
    template<typename T>                                                                        \
    template<typename U>                                                                        \
    typeName VariantContainer<T>::get##typeName(                                                \
        typename std::enable_if<                                                                \
            !std::is_convertible<typename TypeOrEnumType<U>::type, typeName>::value             \
        >::type*                                                                                \
        ) const                                                                                 \
    {                                                                                           \
        return typeName( );                                                                     \
    }                                                                                           \
    template<typename T>                                                                        \
    template<typename U>                                                                        \
    typeName VariantContainer<T>::get##typeName(                                                \
        typename std::enable_if<                                                                \
            std::is_convertible<typename TypeOrEnumType<U>::type, typeName>::value              \
        >::type*                                                                                \
    ) const                                                                                     \
    {                                                                                           \
        return static_cast<typeName>( value_ );                                                 \
    }


#define DECLARE_STANDARD_VARIANT(type)                                     \
    template<>                                                             \
    class VariantContainer<type> : public VariantBase                      \
    {                                                                      \
    public:                                                                \
        VariantContainer(const type &value);                               \
        VariantContainer(const type &&value);                              \
                                                                           \
        Type GetType(void) const override;                                 \
        void *GetPtr(void) const override;                                 \
                                                                           \
        int ToInt(void) const override;                                    \
        bool ToBool(void) const override;                                  \
        float ToFloat(void) const override;                                \
        double ToDouble(void) const override;                              \
        std::string ToString(void) const override;                         \
                                                                           \
        VariantBase *Clone(void) const override;                           \
                                                                           \
    private:                                                               \
        friend class Variant;                                              \
                                                                           \
        VariantContainer &operator=(const VariantContainer &rhs) = delete; \
                                                                           \
        type value_;                                                       \
    };


namespace cytx
{
    namespace meta
    {
        template<typename T>
        VariantContainer<T>::VariantContainer(const NonRefType &value)
            : value_(const_cast<NonRefType&>(value))
        {

        }

        ///////////////////////////////////////////////////////////////////////

        template<typename T>
        VariantContainer<T>::VariantContainer(const NonRefType &&value)
            : value_(std::move(const_cast<NonRefType&&>(value)))
        {

        }

        ///////////////////////////////////////////////////////////////////////

        template<typename T>
        Type VariantContainer<T>::GetType(void) const
        {
            return typeof(T);
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename T>
        int VariantContainer<T>::ToInt(void) const
        {
            return getint();
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename T>
        bool VariantContainer<T>::ToBool(void) const
        {
            return getbool();
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename T>
        float VariantContainer<T>::ToFloat(void) const
        {
            return getfloat();
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename T>
        double VariantContainer<T>::ToDouble(void) const
        {
            return getdouble();
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename T>
        std::string VariantContainer<T>::ToString(void) const
        {
            return getString();
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename T>
        void *VariantContainer<T>::GetPtr(void) const
        {
            return const_cast<void*>(
                reinterpret_cast<const void*>(
                    std::addressof(value_)
                    )
                );
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename T>
        VariantBase *VariantContainer<T>::Clone(void) const
        {
            // use the non reference type to ensure a copy is made
            return new VariantContainer<NonRefType>(value_);
        }

        ///////////////////////////////////////////////////////////////////////

        DEFAULT_TYPE_HANDLER_IMPL(int);
        DEFAULT_TYPE_HANDLER_IMPL(bool);
        DEFAULT_TYPE_HANDLER_IMPL(float);
        DEFAULT_TYPE_HANDLER_IMPL(double);

        template<typename T>
        template<typename U>
        std::string VariantContainer<T>::getString(
            typename std::enable_if<
            !std::is_arithmetic<U>::value
            >::type*
        ) const
        {
            return std::string();
        }

        template<typename T>
        template<typename U>
        std::string VariantContainer<T>::getString(
            typename std::enable_if<
            std::is_arithmetic<U>::value
            >::type*
        ) const
        {
            return std::to_string(m_value);
        }


        // void is a tricky 'ol fella, and must be defined manually
        template<>
        class VariantContainer<void> : public VariantBase
        {
        public:
            VariantContainer(void);

            Type GetType(void) const override;
            void *GetPtr(void) const override;

            int ToInt(void) const override;
            bool ToBool(void) const override;
            float ToFloat(void) const override;
            double ToDouble(void) const override;
            std::string ToString(void) const override;

            VariantBase *Clone(void) const override;

        private:
            friend class Variant;

            VariantContainer &operator=(const VariantContainer &rhs) = delete;
        };

        DECLARE_STANDARD_VARIANT(int);
        DECLARE_STANDARD_VARIANT(unsigned int);
        DECLARE_STANDARD_VARIANT(bool);
        DECLARE_STANDARD_VARIANT(float);
        DECLARE_STANDARD_VARIANT(double);
        DECLARE_STANDARD_VARIANT(std::string);
    }
}

#if defined(COMPILER_MSVC)
#pragma warning(pop)
#endif