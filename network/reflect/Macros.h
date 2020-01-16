#pragma once

#if defined(__CODE_GENERATOR__)

#define Meta(...) __attribute__((annotate(#__VA_ARGS__)))

#define __META_EXTERNAL(type, guid)       \
        typedef type __META_EXTERNAL__##guid; \

#define _META_EXTERNAL(type, guid) __META_EXTERNAL(type, guid)

#define MetaExternal(type) _META_EXTERNAL(type, __COUNTER__)

#define META_OBJECT

#else

#define Meta(...)

#define MetaExternal(type)

#define MetaInitialize(initializer)                                     \
        {                                                               \
            auto &db = cytx::meta::ReflectionDatabase::Instance( );     \
            initializer;                                                \
        }                                                               \

// Used in objects to preserve virtual inheritance functionality
#define META_OBJECT                                         \
        cytx::meta::Type GetType(void) const override       \
        {                                                   \
            return typeof( decltype( *this ) );             \
        }                                                   \
        cytx::meta::Object *Clone(void) const override      \
        {                                                   \
            typedef                                         \
            std::remove_const<                              \
                std::remove_reference<                      \
                    decltype( *this )                       \
                >::type                                     \
            >::type ClassType;                              \
            return new ClassType( *this );                  \
        }                                                   \

#endif
