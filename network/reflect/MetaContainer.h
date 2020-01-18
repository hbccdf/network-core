#pragma once
#include "MetaManager.h"

namespace cytx
{
    namespace meta
    {
        class MetaContainer
        {
        public:
            const MetaManager& GetMeta() const
            {
                return meta_;
            }

        private:
            MetaContainer& operator=(const MetaContainer& rhs) = delete;

            friend class ReflectionDatabase;
            friend struct TypeData;

            MetaManager meta_;
        };
    }
}