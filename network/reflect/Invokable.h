#pragma once
#include <vector>
#include "MetaTraits.h"

namespace cytx
{
    namespace meta
    {
        class Type;

        class Invokable
        {
        public:
            Invokable(const std::string &name = "INVALID")
                : name_(name) { }

            template<typename ...Types>
            static InvokableSignature CreateSignature();

            const InvokableSignature& GetSignature() const
            {
                return signature_;
            }

            const std::string& GetName() const
            {
                return name_;
            }

        protected:
            std::string name_;

            InvokableSignature signature_;
        };
    }
}

#include "Type.h"

namespace std
{
    template<>
    struct hash<cytx::meta::InvokableSignature>
    {
        size_t operator()(const cytx::meta::InvokableSignature &signature) const
        {
            hash<cytx::meta::TypeID> hasher;

            size_t seed = 0;

            // combine the hash of all type IDs in the signature
            for (auto &type : signature)
                seed ^= hasher(type.GetID()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

            return seed;
        }
    };
}

namespace cytx
{
    namespace meta
    {
        template<typename ...Types>
        InvokableSignature Invokable::CreateSignature(void)
        {
            static InvokableSignature signature;

            static auto initial = true;

            if (initial)
            {
                TypeUnpacker<Types...>::Apply(signature);

                initial = false;
            }

            return signature;
        }
    }
}