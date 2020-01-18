#pragma once
#include <unordered_map>
#include "EnumBase.h"
#include "Variant.h"

namespace cytx
{
    namespace meta
    {
        template<typename EnumType>
        class EnumContainer : public EnumBase
        {
        public:
            using Initializer = std::initializer_list<std::pair<std::string, EnumType>>;
            using Table = std::unordered_map<std::string, EnumType>;

            EnumContainer(const std::string &name, const Initializer &initializer)
                : EnumBase(name, InvalidTypeID)
            {
                for (auto &pair : initializer)
                {
                    keys_.emplace_back(pair.first);

                    keyToValue_.emplace(pair);
                }
            }

            EnumContainer(
                const std::string &name,
                const Initializer &initializer,
                TypeID owner
            )
                : EnumBase(name, owner)
            {
                for (auto &pair : initializer)
                {
                    keys_.emplace_back(pair.first);

                    keyToValue_.emplace(pair);
                }
            }

            Type GetType() const override
            {
                return typeof(EnumType);
            }
            Type GetUnderlyingType() const override
            {
                return typeof(typename std::underlying_type<EnumType>::type);
            }

            const std::vector<std::string> &GetKeys() const override
            {
                return keys_;
            }
            std::vector<Variant> GetValues() const override
            {
                std::vector<Variant> values;

                for (auto &entry : keyToValue_)
                    values.emplace_back(entry.second);

                return values;
            }

            std::string GetKey(const Argument& value) const override
            {
                using UnderlyingType = typename std::underlying_type< EnumType >::type;

                auto type = value.GetType();

                // invalid type
                if (type != typeof(EnumType) &&
                    type != typeof(UnderlyingType))
                {
                    return std::string();
                }

                auto &converted = value.GetValue<EnumType>();

                for (auto &entry : keyToValue_)
                {
                    if (entry.second == converted)
                        return entry.first;
                }

                return std::string();
            }
            Variant GetValue(const std::string& key) const override
            {
                auto search = keyToValue_.find(key);

                // not found
                if (search == keyToValue_.end())
                    return Variant();

                return Variant(search->second);
            }

        private:
            Table keyToValue_;
            std::vector<std::string> keys_;
        };
    }
}