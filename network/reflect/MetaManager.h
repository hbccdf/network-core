#pragma once
#include <string>
#include <map>
#include <vector>
#include "MetaProperty.h"

namespace cytx
{
    namespace meta
    {
        class Type;
        class Variant;

        class MetaManager
        {
        public:
            using Initializer = std::initializer_list<std::pair<Type, const MetaProperty*>>;

            using PropertyList = std::vector<Variant>;

            MetaManager();
            MetaManager(const MetaManager& rhs);
            MetaManager(const MetaManager&& rhs);

            const MetaManager& operator=(const MetaManager& rhs);

            MetaManager(const Initializer& properties);

            ~MetaManager();

            Variant GetProperty(Type type) const;

            template<typename PropertyType>
            const PropertyType* GetProperty() const;

            // Sets the given property
            void SetProperty(Type type, const MetaProperty* value);

            // Gets all properties
            PropertyList GetProperties() const;

        private:
            void copy(const MetaManager& rhs);

            std::map<Type, const MetaProperty*> properties_;
        };
    }
}


#include "Type.h"

namespace cytx
{
    namespace meta
    {
        template<typename PropertyType>
        const PropertyType *MetaManager::GetProperty(void) const
        {
            static_assert(std::is_base_of<MetaProperty, PropertyType>::value,
                "Type must be a MetaProperty.");

            static const auto type = typeof(PropertyType);

            auto search = properties_.find(type);

            // doesn't exist
            if (search == properties_.end())
                return nullptr;

            return static_cast<const PropertyType*>(search->second);
        }
    }
}