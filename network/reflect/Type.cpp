#include "Type.h"
#include "TypeCreator.h"
#include "Variant.h"
#include "Enum.h"
#include "Constructor.h"
#include "Destructor.h"
#include "Field.h"
#include "Method.h"
#include "Function.h"
#include "MetaManager.h"
#include "ReflectionDatabase.h"

namespace cytx
{
    namespace meta
    {
        namespace
        {
            // make sure we always have a reference to the gDatabase
            #define gDatabase ReflectionDatabase::Instance( )
        }

        Type::Type(void)
            : id_( InvalidTypeID )
            , isArray_( false ) { }

        ///////////////////////////////////////////////////////////////////////

        Type::Type(const Type &rhs)
            : id_( rhs.id_ )
            , isArray_( rhs.isArray_ ) { }

        ///////////////////////////////////////////////////////////////////////

        Type::Type(TypeID id, bool isArray)
            : id_( id )
            , isArray_( isArray ) { }

        ///////////////////////////////////////////////////////////////////////

        Type::operator bool(void) const
        {
            return id_ != InvalidTypeID;
        }

        ///////////////////////////////////////////////////////////////////////

        Type &Type::operator=(const Type &rhs)
        {
            id_ = rhs.id_;
            isArray_ = rhs.isArray_;

            return *this;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::operator<(const Type &rhs) const
        {
            return id_ < rhs.id_;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::operator>(const Type &rhs) const
        {
            return id_ > rhs.id_;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::operator<=(const Type &rhs) const
        {
            return id_ <= rhs.id_;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::operator>=(const Type &rhs) const
        {
            return id_ >= rhs.id_;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::operator==(const Type &rhs) const
        {
            return id_ == rhs.id_;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::operator!=(const Type &rhs) const
        {
            return id_ != rhs.id_;
        }

        ///////////////////////////////////////////////////////////////////////

        const Type &Type::Invalid(void)
        {
            static const Type invalid { InvalidTypeID };

            return invalid;
        }

        ///////////////////////////////////////////////////////////////////////

        TypeID Type::GetID(void) const
        {
            return id_;
        }

        ///////////////////////////////////////////////////////////////////////

        Type::List Type::GetTypes(void)
        {
            auto count = gDatabase.types.size( );

            List types;

            // skip the first one, as it's reserved for unknown
            for (TypeID i = 1; i < count; ++i)
                types.emplace_back( i );

            return types;
        }

        ///////////////////////////////////////////////////////////////////////

        std::vector<Global> Type::GetGlobals(void)
        {
            std::vector<Global> globals;

            for (auto &global : gDatabase.globals)
                globals.emplace_back( global.second );

            return globals;
        }

        ///////////////////////////////////////////////////////////////////////

        const Global &Type::GetGlobal(const std::string &name)
        {
            return gDatabase.globals[ name ];
        }

        ///////////////////////////////////////////////////////////////////////

        std::vector<Function> Type::GetGlobalFunctions(void)
        {
            std::vector<Function> functions;

            for (auto &overload : gDatabase.globalFunctions)
            {
                for (auto &function : overload.second)
                {
                    functions.emplace_back( function.second );
                }
            }

            return functions;
        }

        ///////////////////////////////////////////////////////////////////////

        const Function &Type::GetGlobalFunction(const std::string &name)
        {
            return gDatabase.GetGlobalFunction( name );
        }

        ///////////////////////////////////////////////////////////////////////

        const Function &Type::GetGlobalFunction(
            const std::string &name,
            const InvokableSignature &signature
        )
        {
            return gDatabase.GetGlobalFunction( name, signature );
        }

        ///////////////////////////////////////////////////////////////////////

        Type Type::GetFromName(const std::string &name)
        {
            auto search = gDatabase.ids.find( name );

            if (search == gDatabase.ids.end( ))
                return Invalid( );

            return { search->second };
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::ListsEqual(const List &a, const List &b)
        {
            if (a.size( ) != b.size( ))
                return false;

            auto equal = true;

            for (size_t i = 0u, size = a.size( ); equal && i < size; ++i)
                equal = (a[ i ] == b[ i ]);

            return equal;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::IsValid(void) const
        {
            return id_ != InvalidTypeID;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::IsPrimitive(void) const
        {
            return gDatabase.types[ id_ ].isPrimitive;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::IsFloatingPoint(void) const
        {
            return gDatabase.types[ id_ ].isFloatingPoint;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::IsSigned(void) const
        {
            return gDatabase.types[ id_ ].isSigned;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::IsEnum(void) const
        {
            return gDatabase.types[ id_ ].isEnum;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::IsPointer(void) const
        {
            return gDatabase.types[ id_ ].isPointer;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::IsClass(void) const
        {
            return gDatabase.types[ id_ ].isClass;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::IsArray(void) const
        {
            return isArray_;
        }

        ///////////////////////////////////////////////////////////////////////

        std::string Type::GetName(void) const
        {
            auto &name = gDatabase.types[ id_ ].name;

            if (IsArray( ))
                return "Array<" + name + ">";

            return name;
        }

        const MetaManager &Type::GetMeta(void) const
        {
            return gDatabase.types[ id_ ].meta;
        }

        ///////////////////////////////////////////////////////////////////////

        void Type::Destroy(Variant &instance) const
        {
            auto &destructor = gDatabase.types[ id_ ].destructor;

            if (destructor.IsValid( ))
                destructor.Invoke( instance );
        }

        ///////////////////////////////////////////////////////////////////////

        Type Type::GetDecayedType(void) const
        {
            //UAssert( false, "Type::GetDecayedType() not implemented." );

            // @@@TODO: convert to non pointer/const pointer type
            return Type( );
        }

        ///////////////////////////////////////////////////////////////////////

        Type Type::GetArrayType(void) const
        {
            return Type( id_, false );
        }

        ///////////////////////////////////////////////////////////////////////

        const Enum &Type::GetEnum(void) const
        {
            return gDatabase.types[ id_ ].enumeration;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::DerivesFrom(const Type &other) const
        {
            auto &baseClasses = gDatabase.types[ id_ ].baseClasses;

            return baseClasses.find( other ) != baseClasses.end( );
        }

        ///////////////////////////////////////////////////////////////////////

        const Type::Set &Type::GetBaseClasses(void) const
        {
            return gDatabase.types[ id_ ].baseClasses;
        }

        ///////////////////////////////////////////////////////////////////////

        const Type::Set &Type::GetDerivedClasses(void) const
        {
            return gDatabase.types[ id_ ].derivedClasses;
        }

        ///////////////////////////////////////////////////////////////////////

        std::vector<Constructor> Type::GetConstructors(void) const
        {
            auto &handle = gDatabase.types[ id_ ].constructors;

            std::vector<Constructor> constructors;

            for (auto &constructor : handle)
                constructors.emplace_back( constructor.second );

            return constructors;
        }

        ///////////////////////////////////////////////////////////////////////

        std::vector<Constructor> Type::GetDynamicConstructors(void) const
        {
            auto &handle = gDatabase.types[ id_ ].dynamicConstructors;

            std::vector<Constructor> constructors;

            for (auto &constructor : handle)
                constructors.emplace_back( constructor.second );

            return constructors;
        }

        ///////////////////////////////////////////////////////////////////////

        const Constructor &Type::GetConstructor(
            const InvokableSignature &signature
        ) const
        {
            return gDatabase.types[ id_ ].GetConstructor( signature );
        }

        ///////////////////////////////////////////////////////////////////////

        const Constructor &Type::GetDynamicConstructor(
            const InvokableSignature &signature
        ) const
        {
            return gDatabase.types[ id_ ].GetDynamicConstructor( signature );
        }

        ///////////////////////////////////////////////////////////////////////

        const Constructor &Type::GetArrayConstructor(void) const
        {
            return gDatabase.types[ id_ ].arrayConstructor;
        }

        ///////////////////////////////////////////////////////////////////////

        const Destructor &Type::GetDestructor(void) const
        {
            return gDatabase.types[ id_ ].destructor;
        }

        ///////////////////////////////////////////////////////////////////////

        std::vector<Method> Type::GetMethods(void) const
        {
            std::vector<Method> methods;

            auto &handle = gDatabase.types[ id_ ].methods;

            for (auto &overload : handle)
            {
                for (auto &method : overload.second)
                {
                    methods.emplace_back( method.second );
                }
            }

            return methods;
        }

        ///////////////////////////////////////////////////////////////////////

        const Method &Type::GetMethod(const std::string &name) const
        {
            return gDatabase.types[ id_ ].GetMethod( name );
        }

        ///////////////////////////////////////////////////////////////////////

        const Method &Type::GetMethod(
            const std::string &name,
            const InvokableSignature &signature
        ) const
        {
            return gDatabase.types[ id_ ].GetMethod( name, signature );
        }

        ///////////////////////////////////////////////////////////////////////

        std::vector<Function> Type::GetStaticMethods(void) const
        {
            std::vector<Function> methods;

            auto &handle = gDatabase.types[ id_ ].staticMethods;

            for (auto &overload : handle)
            {
                for (auto &method : overload.second)
                {
                    methods.emplace_back( method.second );
                }
            }

            return methods;
        }

        ///////////////////////////////////////////////////////////////////////

        const Function &Type::GetStaticMethod(const std::string &name) const
        {
            return gDatabase.types[ id_ ].GetStaticMethod( name );
        }

        ///////////////////////////////////////////////////////////////////////

        const Function &Type::GetStaticMethod(
            const std::string &name,
            const InvokableSignature &signature
        ) const
        {
            return gDatabase.types[ id_ ].GetStaticMethod( name, signature );
        }

        ///////////////////////////////////////////////////////////////////////

        const std::vector<Field> &Type::GetFields(void) const
        {
            // @@@TODO: recursively get base class fields

            return gDatabase.types[ id_ ].fields;
        }

        ///////////////////////////////////////////////////////////////////////

        const Field &Type::GetField(const std::string &name) const
        {
            return gDatabase.types[ id_ ].GetField( name );
        }

        ///////////////////////////////////////////////////////////////////////

        std::vector<Global> Type::GetStaticFields(void) const
        {
            return gDatabase.types[ id_ ].staticFields;
        }

        ///////////////////////////////////////////////////////////////////////

        const Global &Type::GetStaticField(const std::string &name) const
        {
            return gDatabase.types[ id_ ].GetStaticField( name );
        }
    }
}
