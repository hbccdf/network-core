#pragma once
#include "VariantBase.h"

namespace cytx
{
    namespace meta
    {
        class Object;

        class ObjectWrapper : public VariantBase
        {
        public:
            ObjectWrapper(Object* instance);

            Type GetType() const override;
            void *GetPtr() const override;

            int ToInt() const override;
            bool ToBool() const override;
            float ToFloat() const override;
            double ToDouble() const override;
            std::string ToString() const override;

            VariantBase* Clone() const override;
        private:
            Object* object_;
        };
    }
}