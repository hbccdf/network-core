#pragma once

#include <boost/serialization/singleton.hpp>
#include <boost/pool/pool.hpp>
#include <boost/thread/mutex.hpp>
#include <vector>

namespace cytx
{
#define NEW_MP(type, ...)							MemoryPoolManager::get_mutable_instance().new_memorypool<type>(sizeof(type), 1, ##__VA_ARGS__)
#define NEW_ARRAY_MP(type, length, ...)				MemoryPoolManager::get_mutable_instance().new_memorypool<type>(sizeof(type) * length, length, ##__VA_ARGS__)
#define DELETE_MP(type, ptr)						MemoryPoolManager::get_mutable_instance().delete_memorypool<type>(ptr, sizeof(type), 1)
#define DELETE_ARRAY_MP(type, ptr, length)			MemoryPoolManager::get_mutable_instance().delete_memorypool<type>(ptr, sizeof(type) * length, length)

    class MemoryPoolManager : public boost::serialization::singleton<MemoryPoolManager>
    {
    public:
        static inline void malloc_faild()
        {
            throw std::bad_alloc();
        }

        void init()
        {
            if (is_init_)
                return;

            is_init_ = true;
            std::set_new_handler(malloc_faild);

            std::size_t memory_pool_size = 1;
            for (int index = 0; index < 16; memory_pool_size <<= 1, ++index)
            {
                boost::pool<>* memory_pool = new boost::pool<>(memory_pool_size);
                map_memory_pool.insert(std::pair<const std::size_t, boost::pool <>* >(memory_pool_size, memory_pool));

#ifdef _DEBUG
                std::vector<void*> debug_vector;
                debug_map_memory_pool.insert(std::pair<const std::size_t, std::vector<void*> >(memory_pool_size, debug_vector));
#endif
            }
        }

        void* malloc_memorypool(const std::size_t alloc_size)
        {
            void* ptr = NULL;

            boost::mutex::scoped_lock tempx_mutex_memory_pool(mutex_memory_pool);
            std::map<const std::size_t, boost::pool <>* >::iterator it_memory_pool = map_memory_pool.begin();
            for (; it_memory_pool->first < alloc_size; )
            {
                if (++it_memory_pool == map_memory_pool.end())
                {
                    return (malloc_memorypool_overflow)(alloc_size);
                }
            }

            for (; (ptr = it_memory_pool->second->malloc()) == 0; )
            {
                if (++it_memory_pool == map_memory_pool.end())
                {
                    return (malloc_memorypool_overflow)(alloc_size);
                }
            }

#ifdef _DEBUG
            std::map<const std::size_t, std::vector<void*> >::iterator debug_it_memory_pool = debug_map_memory_pool.find(it_memory_pool->first);
            debug_it_memory_pool->second.push_back(ptr);
#endif

            return ptr;
        }

        void* malloc_memorypool_overflow(const std::size_t alloc_size)
        {
            void* ptr = NULL;
            while ((ptr = std::malloc(alloc_size)) == NULL)
            {
                (malloc_faild)();
            }

#ifdef _DEBUG
            std::map<const std::size_t, std::vector<void*> >::iterator debug_it_memory_pool = debug_map_memory_pool.find(alloc_size);
            if (debug_it_memory_pool != debug_map_memory_pool.end())
            {
                debug_it_memory_pool->second.push_back(ptr);
            }
            else
            {
                std::vector<void*> debug_vector;
                debug_vector.push_back(ptr);
                debug_map_memory_pool.insert(std::pair<const std::size_t, std::vector<void*> >(alloc_size, debug_vector));
            }
#endif

            return ptr;
        }

        void free_memorypool(void* ptr, const std::size_t alloc_size)
        {
            boost::mutex::scoped_lock tempx_mutex_memory_pool(mutex_memory_pool);

            std::map<const std::size_t, boost::pool <>* >::iterator it_memory_pool = map_memory_pool.begin();
            for (; it_memory_pool != map_memory_pool.end(); ++it_memory_pool)
            {
                if (alloc_size <= it_memory_pool->first && it_memory_pool->second->is_from(ptr))
                {
#ifdef _DEBUG
                    std::map<const std::size_t, std::vector<void*> >::iterator debug_it_memory_pool = debug_map_memory_pool.find(it_memory_pool->first);

                    bool is_already_erase = true;
                    std::vector<void*>::iterator debug_it_vector = debug_it_memory_pool->second.begin();
                    for (; debug_it_vector != debug_it_memory_pool->second.end(); ++debug_it_vector)
                    {
                        if (*debug_it_vector == ptr)
                        {
                            debug_it_memory_pool->second.erase(debug_it_vector);
                            is_already_erase = false;
                            break;
                        }
                    }
#endif

                    return it_memory_pool->second->free(ptr);
                }
            }

            (free_memorypool_overflow)(ptr, alloc_size);
        }

        void free_memorypool_overflow(void* ptr, const std::size_t alloc_size)
        {
#ifdef _DEBUG
            std::map<const std::size_t, std::vector<void*> >::iterator debug_it_memory_pool = debug_map_memory_pool.find(alloc_size);
            std::vector<void*>::iterator debug_it_vector = debug_it_memory_pool->second.begin();
            for (; debug_it_vector != debug_it_memory_pool->second.end(); ++debug_it_vector)
            {
                if (*debug_it_vector == ptr)
                {
                    debug_it_memory_pool->second.erase(debug_it_vector);
                    break;
                }
            }
#endif

            (std::free)(ptr);
        }

        template <typename T>
        T* new_memorypool(const std::size_t alloc_size, int length)
        {
            std::size_t temp_alloc_size = (alloc_size == 0 ? 1 : alloc_size);

            T* ptr = NULL;
            while ((ptr = static_cast<T*>(malloc_memorypool(temp_alloc_size))) == NULL)
                //if (_callnewh(alloc_size) == 0)
            {
                (malloc_faild)();
            }

            T* temp_ptr = ptr;
            while (length--)
            {
                try { new (temp_ptr) T(); }
                catch (...) { (free_memorypool)(ptr, alloc_size); throw; }
                ++temp_ptr;
            }

            return ptr;
        }

        template <typename T, class Arg1>
        T* new_memorypool(const std::size_t alloc_size, int length, Arg1 arg1)
        {
            std::size_t temp_alloc_size = (alloc_size == 0 ? 1 : alloc_size);

            T* ptr = NULL;
            while ((ptr = static_cast<T*>(malloc_memorypool(temp_alloc_size))) == NULL)
                //if (_callnewh(alloc_size) == 0)
            {
                (malloc_faild)();
            }

            T* temp_ptr = ptr;
            while (length--)
            {
                try { new (temp_ptr) T(arg1); }
                catch (...) { (free_memorypool)(ptr, alloc_size); throw; }
                ++temp_ptr;
            }

            return ptr;
        }

        template <typename T, class Arg1, class Arg2>
        T* new_memorypool(const std::size_t alloc_size, int length, Arg1 arg1, Arg2 arg2)
        {
            std::size_t temp_alloc_size = (alloc_size == 0 ? 1 : alloc_size);

            T* ptr = NULL;
            while ((ptr = static_cast<T*>(malloc_memorypool(temp_alloc_size))) == NULL)
                //if (_callnewh(alloc_size) == 0)
            {
                (malloc_faild)();
            }

            T* temp_ptr = ptr;
            while (length--)
            {
                try { new (temp_ptr) T(arg1, arg2); }
                catch (...) { (free_memorypool)(ptr, alloc_size); throw; }
                ++temp_ptr;
            }

            return ptr;
        }

        template <typename T, class Arg1, class Arg2, class Arg3>
        T* new_memorypool(const std::size_t alloc_size, int length, Arg1 arg1, Arg2 arg2, Arg3 arg3)
        {
            std::size_t temp_alloc_size = (alloc_size == 0 ? 1 : alloc_size);

            T* ptr = NULL;
            while ((ptr = static_cast<T*>(malloc_memorypool(temp_alloc_size))) == NULL)
                //if (_callnewh(alloc_size) == 0)
            {
                (malloc_faild)();
            }

            T* temp_ptr = ptr;
            while (length--)
            {
                try { new (temp_ptr) T(arg1, arg2, arg3); }
                catch (...) { (free_memorypool)(ptr, alloc_size); throw; }
                ++temp_ptr;
            }

            return ptr;
        }

        template <typename T, class Arg1, class Arg2, class Arg3, class Arg4>
        T* new_memorypool(const std::size_t alloc_size, int length, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
        {
            std::size_t temp_alloc_size = (alloc_size == 0 ? 1 : alloc_size);

            T* ptr = NULL;
            while ((ptr = static_cast<T*>(malloc_memorypool(temp_alloc_size))) == NULL)
                //if (_callnewh(alloc_size) == 0)
            {
                (malloc_faild)();
            }

            T* temp_ptr = ptr;
            while (length--)
            {
                try { new (temp_ptr) T(arg1, arg2, arg3, arg4); }
                catch (...) { (free_memorypool)(ptr, alloc_size); throw; }
                ++temp_ptr;
            }

            return ptr;
        }

        template <typename T, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
        T* new_memorypool(const std::size_t alloc_size, int length, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5)
        {
            std::size_t temp_alloc_size = (alloc_size == 0 ? 1 : alloc_size);

            T* ptr = NULL;
            while ((ptr = static_cast<T*>(malloc_memorypool(temp_alloc_size))) == NULL)
                //if (_callnewh(alloc_size) == 0)
            {
                (malloc_faild)();
            }

            T* temp_ptr = ptr;
            while (length--)
            {
                try { new (temp_ptr) T(arg1, arg2, arg3, arg4, arg5); }
                catch (...) { (free_memorypool)(ptr, alloc_size); throw; }
                ++temp_ptr;
            }

            return ptr;
        }

        template <typename T>
        void delete_memorypool(T* ptr, const std::size_t alloc_size, int length)
        {
            if (ptr == NULL) return;

            T* temp_ptr = ptr;
            while (length--)
            {
                temp_ptr->~T();
                ++temp_ptr;
            }

            (free_memorypool)(ptr, alloc_size);
            ptr = NULL;
        }

#ifdef _DEBUG
        void debug_map_memory_pool_function()
        {
            std::map<const std::size_t, std::vector<void*> >::iterator debug_it_memory_pool = debug_map_memory_pool.begin();
            for (; debug_it_memory_pool != debug_map_memory_pool.end(); ++debug_it_memory_pool)
            {
                std::vector<void*>::iterator debug_it_vector = debug_it_memory_pool->second.begin();
                for (; debug_it_vector != debug_it_memory_pool->second.end(); ++debug_it_vector)
                {
                }
            }
        }
#endif

    public:
        boost::mutex mutex_memory_pool;
        std::map<const std::size_t, boost::pool <>* > map_memory_pool;		// min : 1	max : 32768

#ifdef _DEBUG
        std::map<const std::size_t, std::vector<void*> > debug_map_memory_pool;
#endif

        bool is_init_ = false;
    };
}
