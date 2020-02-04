#include "VariantContainer.h"
#include "Type.h"
#include <algorithm>

namespace cytx
{
    namespace meta
    {
        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        // void
        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////

        #pragma region void

        VariantContainer<void>::VariantContainer(void) { }

        Type VariantContainer<void>::GetType(void) const
        {
            return typeof( void );
        }

        ///////////////////////////////////////////////////////////////////////

        void *VariantContainer<void>::GetPtr(void) const
        {
            return nullptr;
        }

        ///////////////////////////////////////////////////////////////////////

        int VariantContainer<void>::ToInt(void) const
        {
            return int( );
        }

        ///////////////////////////////////////////////////////////////////////

        bool VariantContainer<void>::ToBool(void) const
        {
            return bool( );
        }

        ///////////////////////////////////////////////////////////////////////

        float VariantContainer<void>::ToFloat(void) const
        {
            return float( );
        }

        ///////////////////////////////////////////////////////////////////////

        double VariantContainer<void>::ToDouble(void) const
        {
            return double( );
        }

        ///////////////////////////////////////////////////////////////////////

        std::string VariantContainer<void>::ToString(void) const
        {
            return std::string( );
        }

        ///////////////////////////////////////////////////////////////////////

        VariantBase *VariantContainer<void>::Clone(void) const
        {
            return new VariantContainer<void>( );
        }

        #pragma endregion

        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        // int
        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////

        #pragma region int

        VariantContainer<int>::VariantContainer(const int &value)
            : value_( value ) { }

        ///////////////////////////////////////////////////////////////////////

        VariantContainer<int>::VariantContainer(const int &&value)
            : value_( std::move( value ) ) { }

        ///////////////////////////////////////////////////////////////////////

        Type VariantContainer<int>::GetType(void) const
        {
            return typeof( int );
        }

        ///////////////////////////////////////////////////////////////////////

        void *VariantContainer<int>::GetPtr(void) const
        {
            return const_cast<void*>(
                reinterpret_cast<const void*>(
                    std::addressof( value_ )
                )
            );
        }

        ///////////////////////////////////////////////////////////////////////

        int VariantContainer<int>::ToInt(void) const
        {
            return value_;
        }

        ///////////////////////////////////////////////////////////////////////

        bool VariantContainer<int>::ToBool(void) const
        {
            return value_ == 0 ? false : true;
        }

        ///////////////////////////////////////////////////////////////////////

        float VariantContainer<int>::ToFloat(void) const
        {
            return static_cast<float>( value_ );
        }

        ///////////////////////////////////////////////////////////////////////

        double VariantContainer<int>::ToDouble(void) const
        {
            return static_cast<double>( value_ );
        }

        ///////////////////////////////////////////////////////////////////////

        std::string VariantContainer<int>::ToString(void) const
        {
            return std::to_string( value_ );
        }

        ///////////////////////////////////////////////////////////////////////

        VariantBase *VariantContainer<int>::Clone(void) const
        {
            return new VariantContainer<int>( value_ );
        }

        #pragma endregion

        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        // unsigned int
        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////

        #pragma region unsigned unsigned

        VariantContainer<unsigned>::VariantContainer(const unsigned &value)
            : value_( value ) { }

        ///////////////////////////////////////////////////////////////////////

        VariantContainer<unsigned>::VariantContainer(const unsigned &&value)
            : value_( std::move( value ) ) { }

        ///////////////////////////////////////////////////////////////////////

        Type VariantContainer<unsigned>::GetType(void) const
        {
            return typeof( unsigned );
        }

        ///////////////////////////////////////////////////////////////////////

        void *VariantContainer<unsigned>::GetPtr(void) const
        {
            return const_cast<void*>(
                reinterpret_cast<const void*>(
                    std::addressof( value_ )
                )
            );
        }

        ///////////////////////////////////////////////////////////////////////

        int VariantContainer<unsigned>::ToInt(void) const
        {
            return static_cast<int>( value_ );
        }

        ///////////////////////////////////////////////////////////////////////

        bool VariantContainer<unsigned>::ToBool(void) const
        {
            return value_ == 0 ? false : true;
        }

        ///////////////////////////////////////////////////////////////////////

        float VariantContainer<unsigned>::ToFloat(void) const
        {
            return static_cast<float>( value_ );
        }

        ///////////////////////////////////////////////////////////////////////

        double VariantContainer<unsigned>::ToDouble(void) const
        {
            return static_cast<double>( value_ );
        }

        ///////////////////////////////////////////////////////////////////////

        std::string VariantContainer<unsigned>::ToString(void) const
        {
            return std::to_string( value_ );
        }

        ///////////////////////////////////////////////////////////////////////

        VariantBase *VariantContainer<unsigned>::Clone(void) const
        {
            return new VariantContainer<unsigned>( value_ );
        }

        #pragma endregion

        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        // bool
        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////

        #pragma region bool

        VariantContainer<bool>::VariantContainer(const bool &value)
            : value_( value ) { }

        ///////////////////////////////////////////////////////////////////////

        VariantContainer<bool>::VariantContainer(const bool &&value)
            : value_( std::move( value ) ) { }

        ///////////////////////////////////////////////////////////////////////

        Type VariantContainer<bool>::GetType(void) const
        {
            return typeof( bool );
        }

        ///////////////////////////////////////////////////////////////////////

        void *VariantContainer<bool>::GetPtr(void) const
        {
            return const_cast<void*>(
                reinterpret_cast<const void*>(
                    std::addressof( value_ )
                )
            );
        }

        ///////////////////////////////////////////////////////////////////////

        int VariantContainer<bool>::ToInt(void) const
        {
            return value_ ? 1 : 0;
        }

        ///////////////////////////////////////////////////////////////////////

        bool VariantContainer<bool>::ToBool(void) const
        {
            return value_;
        }

        ///////////////////////////////////////////////////////////////////////

        float VariantContainer<bool>::ToFloat(void) const
        {
            return value_ ? 1.0f : 0.0f;
        }

        ///////////////////////////////////////////////////////////////////////

        double VariantContainer<bool>::ToDouble(void) const
        {
            return value_ ? 1.0 : 0.0;
        }

        ///////////////////////////////////////////////////////////////////////

        std::string VariantContainer<bool>::ToString(void) const
        {
            return value_ ? "true" : "false";
        }

        ///////////////////////////////////////////////////////////////////////

        VariantBase *VariantContainer<bool>::Clone(void) const
        {
            return new VariantContainer<bool>( value_ );
        }

        #pragma endregion

        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        // float
        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////

        #pragma region float

        VariantContainer<float>::VariantContainer(const float &value)
            : value_( value ) { }

        ///////////////////////////////////////////////////////////////////////

        VariantContainer<float>::VariantContainer(const float &&value)
            : value_( std::move( value ) ) { }

        ///////////////////////////////////////////////////////////////////////

        Type VariantContainer<float>::GetType(void) const
        {
            return typeof( float );
        }

        ///////////////////////////////////////////////////////////////////////

        void *VariantContainer<float>::GetPtr(void) const
        {
            return const_cast<void*>(
                reinterpret_cast<const void*>(
                    std::addressof( value_ )
                )
            );
        }

        ///////////////////////////////////////////////////////////////////////

        int VariantContainer<float>::ToInt(void) const
        {
            return static_cast<int>( value_ );
        }

        ///////////////////////////////////////////////////////////////////////

        bool VariantContainer<float>::ToBool(void) const
        {
            return value_ == 0.0f ? false : true;
        }

        ///////////////////////////////////////////////////////////////////////

        float VariantContainer<float>::ToFloat(void) const
        {
            return value_;
        }

        ///////////////////////////////////////////////////////////////////////

        double VariantContainer<float>::ToDouble(void) const
        {
            return static_cast<double>( value_ );
        }

        ///////////////////////////////////////////////////////////////////////

        std::string VariantContainer<float>::ToString(void) const
        {
            return std::to_string( value_ );
        }

        ///////////////////////////////////////////////////////////////////////

        VariantBase *VariantContainer<float>::Clone(void) const
        {
            return new VariantContainer<float>( value_ );
        }

        #pragma endregion

        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        // double
        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////

        #pragma region double

        VariantContainer<double>::VariantContainer(const double &value)
            : value_( value ) { }

        ///////////////////////////////////////////////////////////////////////

        VariantContainer<double>::VariantContainer(const double &&value)
            : value_( std::move( value ) ) { }

        ///////////////////////////////////////////////////////////////////////

        Type VariantContainer<double>::GetType(void) const
        {
            return typeof( double );
        }

        ///////////////////////////////////////////////////////////////////////

        void *VariantContainer<double>::GetPtr(void) const
        {
            return const_cast<void*>(
                reinterpret_cast<const void*>(
                    std::addressof( value_ )
                )
            );
        }

        ///////////////////////////////////////////////////////////////////////

        int VariantContainer<double>::ToInt(void) const
        {
            return static_cast<int>( value_ );
        }

        ///////////////////////////////////////////////////////////////////////

        bool VariantContainer<double>::ToBool(void) const
        {
            return value_ == 0.0 ? false : true;
        }

        ///////////////////////////////////////////////////////////////////////

        float VariantContainer<double>::ToFloat(void) const
        {
            return static_cast<float>( value_ );
        }

        ///////////////////////////////////////////////////////////////////////

        double VariantContainer<double>::ToDouble(void) const
        {
            return value_;
        }

        ///////////////////////////////////////////////////////////////////////

        std::string VariantContainer<double>::ToString(void) const
        {
            return std::to_string( value_ );
        }

        ///////////////////////////////////////////////////////////////////////

        VariantBase *VariantContainer<double>::Clone(void) const
        {
            return new VariantContainer<double>( value_ );
        }

        #pragma endregion

        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        // string
        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////

        #pragma region string

        VariantContainer<std::string>::VariantContainer(
            const std::string &value
        )
            : value_( value ) { }

        ///////////////////////////////////////////////////////////////////////

        VariantContainer<std::string>::VariantContainer(
            const std::string &&value
        )
            : value_( std::move( value ) ) { }

        ///////////////////////////////////////////////////////////////////////

        Type VariantContainer<std::string>::GetType(void) const
        {
            return typeof( std::string );
        }

        ///////////////////////////////////////////////////////////////////////

        void *VariantContainer<std::string>::GetPtr(void) const
        {
            return const_cast<void*>(
                reinterpret_cast<const void*>(
                    std::addressof( value_ )
                )
            );
        }

        ///////////////////////////////////////////////////////////////////////

        int VariantContainer<std::string>::ToInt(void) const
        {
            return stoi( value_ );
        }

        ///////////////////////////////////////////////////////////////////////

        bool VariantContainer<std::string>::ToBool(void) const
        {
            // 0 -> false
            // 1 -> true
            // "true" -> true (case insensitive)
            // "false" -> false (case insensitive)

            if (value_ == "0")
                return false;

            if (value_ == "1")
                return true;

            auto copy = value_;

            // convert to lowercase
            transform( copy.begin( ), copy.end( ), copy.begin( ), tolower );

            if (copy == "true")
                return true;

            return false;
        }

        ///////////////////////////////////////////////////////////////////////

        float VariantContainer<std::string>::ToFloat(void) const
        {
            return stof( value_ );
        }

        ///////////////////////////////////////////////////////////////////////

        double VariantContainer<std::string>::ToDouble(void) const
        {
            return stod( value_ );
        }

        ///////////////////////////////////////////////////////////////////////

        std::string VariantContainer<std::string>::ToString(void) const
        {
            return value_;
        }

        ///////////////////////////////////////////////////////////////////////

        VariantBase *VariantContainer<std::string>::Clone(void) const
        {
            return new VariantContainer<std::string>( value_ );
        }

        #pragma endregion
    }
}
