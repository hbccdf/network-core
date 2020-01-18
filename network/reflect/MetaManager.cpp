#include "MetaManager.h"
#include "Type.h"
#include "Variant.h"
#include <algorithm>

namespace cytx
{
    namespace meta
    {
        MetaManager::MetaManager(void) { }

        MetaManager::MetaManager(const MetaManager &rhs)
        {
            copy( rhs );
        }

        MetaManager::MetaManager(const MetaManager &&rhs)
            : properties_( std::move( rhs.properties_ ) )
        {

        }

        const MetaManager &MetaManager::operator=(const MetaManager &rhs)
        {
            copy( rhs );

            return *this;
        }

        ///////////////////////////////////////////////////////////////////////

        MetaManager::MetaManager(const Initializer &properties)
        {
            for (auto &prop : properties)
            {
                /*UAssert( prop.first.IsValid( ),
                    "Invalid meta property registered."
                    "Make sure the property type is enabled in reflection."
                );*/

                SetProperty( prop.first, prop.second );
            }
        }

        MetaManager::~MetaManager(void)
        {
            for (auto &prop : properties_)
                delete prop.second;
        }

        ///////////////////////////////////////////////////////////////////////

        Variant MetaManager::GetProperty(Type type) const
        {
            auto search = properties_.find( type );

            if (search == properties_.end( ))
                return { };

            return ObjectVariant( search->second );
        }

        ///////////////////////////////////////////////////////////////////////

        void MetaManager::SetProperty(Type type, const MetaProperty *prop)
        {
            auto search = properties_.find( type );

            // delete old property if it exists
            if (search != properties_.end( ))
                delete search->second;

            properties_[ type ] = prop;
        }

        MetaManager::PropertyList MetaManager::GetProperties(void) const
        {
            PropertyList properties;

            for (auto &property : properties_)
                properties.emplace_back( ObjectVariant( property.second ) );

            return properties;
        }

        void MetaManager::copy(const MetaManager &rhs)
        {
            for (auto &prop : rhs.properties_)
            {
                properties_[ prop.first ] =
                    static_cast<MetaProperty*>( prop.second->Clone( ) );
            }
        }
    }
}
