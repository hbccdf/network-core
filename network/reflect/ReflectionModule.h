#pragma once
#include "ReflectionDatabase.h"

#define DECLARE_REFLECTION_MODULE(module)                            \
    namespace meta_generated                                         \
    {                                                                \
        class Module##module : public cytx::meta::ReflectionModule \
        {                                                            \
        public:                                                      \
             Module##module(cytx::meta::ReflectionDatabase &db);   \
            ~Module##module(void);                                   \
        };                                                           \
    }                                                                \

#define UsingModule(module) meta_generated::Module##module _##module( db );

namespace cytx
{
    namespace meta
    {
        class ReflectionModule
        {
        public:
            ReflectionModule(ReflectionDatabase &db)
                : db(db) {}

        protected:
            ReflectionDatabase &db;
        };
    }
}