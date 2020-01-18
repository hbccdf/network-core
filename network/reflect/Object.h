#pragma once

// Constructs a variant that wraps an object
#define ObjectVariant(object) cytx::meta::Variant { object, cytx::meta::variant_policy::WrapObject( ) }

namespace cytx
{
    namespace meta
    {
        class Type;

        class Object
        {
        public:
            virtual ~Object(void) { }

            virtual Type GetType(void) const = 0;
            virtual Object *Clone(void) const = 0;
        };
    }
}