#pragma once

#define NEW_MP(type, ...)							memory_pool::ins().new_memory<type>(sizeof(type), 1, ##__VA_ARGS__)
#define NEW_ARRAY_MP(type, length, ...)				memory_pool::ins().new_memory<type>(sizeof(type) * length, length, ##__VA_ARGS__)
#define DELETE_MP(type, ptr)						memory_pool::ins().delete_memory<type>(ptr)
#define DELETE_ARRAY_MP(type, ptr)			        memory_pool::ins().delete_memory<type>(ptr)

#define MALLOC_MP(type, size)                       (type*)memory_pool::ins().malloc_memory(sizeof(type) * (size))
#define FREE_MP(ptr)                                memory_pool::ins().free_memory(ptr)

#include <boost/pool/pool.hpp>
#include <vector>
#include <map>
#include <mutex>

namespace cytx
{
    class memory_pool
    {
        using lock_t = std::unique_lock<std::mutex>;
        using map_t = std::map<std::size_t, boost::pool<>* >;
        using iterator_t = map_t::iterator;

    public:
        static memory_pool& ins()
        {
            static memory_pool pool;
            return pool;
        }

    public:
        void init()
        {
            if (is_init_)
                return;

            is_init_ = true;
            std::set_new_handler(malloc_faild);

            size_t memory_pool_size = 4;
            for (int index = 0; index < 16; memory_pool_size <<= 1, ++index)
            {
                auto memory_pool = new boost::pool<>(memory_pool_size);
                pool_map_.emplace(memory_pool_size, memory_pool);
                max_memory_pool_size_ = memory_pool_size;
            }
        }

        void* malloc_memory(size_t alloc_size)
        {
            void* ptr = nullptr;

            lock_t locker(mutex_);

            size_t internal_size = get_internal_size(alloc_size);

            auto it = get_iterator(internal_size);
            if (it == pool_map_.end())
            {
                return default_malloc(alloc_size);
            }

            for (; (ptr = it->second->malloc()) == nullptr; )
            {
                if (++it == pool_map_.end())
                {
                    return default_malloc(alloc_size);
                }
            }

            return get_ptr(ptr, internal_size);
        }

        void* default_malloc(size_t alloc_size)
        {
            void* ptr = nullptr;
            while ((ptr = ::malloc(alloc_size)) == nullptr)
            {
                malloc_faild();
            }

            return ptr;
        }

        //ptr 不一定是 alloc_size 对应大小的内存池 分配的
        void free_memory(void* ptr)
        {
            size_t internal_size = get_memory_size(ptr);
			if (internal_size < max_memory_pool_size_)
			{
                lock_t locker(mutex_);
                auto it = get_iterator(internal_size);
                while (it != pool_map_.end())
                {
                    if (it->second->is_from(ptr))
                        return it->second->free(ptr);
                    else
                        ++it;
                }
			}

            default_free(ptr);
        }

        void default_free(void* ptr)
        {
            ::free(ptr);
        }

        template <typename T, typename ... Args>
        T* new_memory(size_t alloc_size, int length, Args&& ... args)
        {
            size_t temp_alloc_size = (alloc_size == 0 ? 1 : alloc_size) + sizeof(length);

            void* ptr = nullptr;
            while ((ptr = malloc_memory(temp_alloc_size)) == nullptr)
            {
                malloc_faild();
            }

            int* target_ptr = (int*)ptr;
            *target_ptr = length;
            ++target_ptr;

            T* temp_ptr = (T*)target_ptr;
            while (length--)
            {
                try
                {
                    new (temp_ptr) T(std::forward<Args>(args) ... );
                }
                catch (...)
                {
                    free_memory(ptr);
                    throw;
                }
                ++temp_ptr;
            }

            return target_ptr;
        }

        template <typename T>
        void delete_memory(T* ptr)
        {
            if (ptr == nullptr)
                return;

            int* target_ptr = (int*)ptr;
            --target_ptr;
            int length = *target_ptr;

            T* temp_ptr = ptr;
            while (length--)
            {
                temp_ptr->~T();
                ++temp_ptr;
            }

            free_memory(target_ptr);
        }

    private:
        static inline void malloc_faild()
        {
            throw std::bad_alloc();
        }

        iterator_t get_iterator(size_t alloc_size)
        {
            for (auto it = pool_map_.begin(); it != pool_map_.end(); ++it)
            {
                if(it->first < alloc_size)
                    continue;

                return it;
            }

            return pool_map_.end();
        }

        size_t get_internal_size(size_t alloc_size) const
        {
            return alloc_size + _memory_head_size;
        }

        void* get_ptr(void* internal_ptr, size_t internal_size) const
        {
            *(int*)internal_ptr = (int)internal_size;
            return (char*)internal_ptr + _memory_head_size;
        }

        void* get_internal_ptr(void* ptr) const
        {
            return (char*)ptr - _memory_head_size;
        }

        size_t get_memory_size(void* ptr) const
        {
            void* internal_ptr = get_internal_ptr(ptr);
            return *(int*)internal_ptr;
        }

    private:
        std::mutex mutex_;
        std::map<std::size_t, boost::pool<>* > pool_map_;		// min : 1	max : 32768

        bool is_init_ = false;
        size_t max_memory_pool_size_ = 0;

        constexpr static int _memory_head_size = 4;
    };
}
