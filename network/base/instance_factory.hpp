#pragma once
#include <unordered_map>
#include "type_id.hpp"

namespace cytx
{
    namespace detail
    {
        class iobj
        {
        public:
            virtual ~iobj() {}

            virtual type_id_t get_id() const = 0;
        };

        template<typename T>
        class iobj_helper : public iobj
        {
            using this_t = iobj_helper;
        public:
            iobj_helper()
                : iobj()
                , val_(new T())
            {}

            virtual ~iobj_helper()
            {
                delete val_;
            }

            T* get_val() const
            {
                return val_;
            }

            type_id_t get_id() const override
            {
                return TypeId::id<this_t>();
            }

        private:
            T* val_;
        };
    }

    class instance_factory
    {
    public:
        static instance_factory& ins()
        {
            static instance_factory factory;
            return factory;
        }
    public:
        template<typename T>
        auto get()
        {
            using real_type = detail::iobj_helper<T>;

            type_id_t tid = TypeId::id<real_type>();
            auto it = type_to_objs_.find(tid);
            if (it != type_to_objs_.end())
            {
                real_type* real_obj = reinterpret_cast<real_type*>(it->second);
                return real_obj->get_val();
            }

            real_type* obj = new real_type();
            type_to_objs_[tid] = obj;
            return obj->get_val();
        }

    private:
        std::unordered_map<type_id_t, detail::iobj*> type_to_objs_;
    };
}

