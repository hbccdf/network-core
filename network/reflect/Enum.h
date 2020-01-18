#pragma once
#include <memory>
#include "EnumBase.h"

namespace cytx
{
    namespace meta
    {
        class Enum
        {
        public:
            bool IsValid() const;

            operator bool() const;

            bool operator ==(const Enum& rhs) const;
            bool operator !=(const Enum& rhs) const;

            std::string GetName() const;

            Type GetType() const;
            Type GetParentType() const;
            Type GetUnderlyingType() const;

            std::vector<std::string> GetKeys() const;
            std::vector<Variant> GetValues() const;

            std::string GetKey(const Argument& value) const;
            Variant GetValue(const std::string& key) const;

        private:
            friend struct TypeData;

            Enum(const EnumBase* base);

            std::shared_ptr<const EnumBase> base_;
        };
    }
}