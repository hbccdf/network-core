#pragma once
#include <string>
#include <vector>
#include "Type.h"

namespace cytx
{
    namespace meta
    {
        class Variant;
        class Argument;

        class EnumBase
        {
        public:
            EnumBase(const std::string& name, TypeID owner);

            virtual ~EnumBase(void) { }

            virtual Type GetType() const = 0;
            virtual Type GetUnderlyingType() const = 0;

            virtual const std::vector<std::string>& GetKeys() const = 0;
            virtual std::vector<Variant> GetValues() const = 0;

            virtual std::string GetKey(const Argument& value) const = 0;
            virtual Variant GetValue(const std::string& key) const = 0;

            Type GetParentType() const;

            const std::string& GetName() const;

        private:
            Type parentType_;

            std::string name_;
        };
    }
}