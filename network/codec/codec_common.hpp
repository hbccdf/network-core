#pragma once
#include "network/base/GameObjectStream.h"
namespace cytx
{
    namespace codec
    {
        struct gos_buffer
        {
            gos_buffer(const gos_buffer& gos) = delete;
            gos_buffer& operator =(const gos_buffer& gos) = delete;

            gos_buffer() { data_ = nullptr; size_ = 0; }
            gos_buffer(gos_buffer&& gos)
            {
                data_ = gos.data_;
                size_ = gos.size_;
                gos.data_ = nullptr;
                gos.size_ = 0;
            }
            gos_buffer(const char* const begin, const char* const end)
                : gos_buffer(begin, end - begin)
            {
            }
            gos_buffer(const char* const begin, size_t size)
            {
                size_ = size;
                data_ = NEW_ARRAY_MP(char, (int)size_);
                memcpy(data_, begin, size_);
            }
            gos_buffer(GameObjectStream& gos)
            {
                data_ = gos.data();
                size_ = gos.length();
                gos.set_alloc_type(0);
            }
            ~gos_buffer()
            {
                if (data_)
                {
                    DELETE_ARRAY_MP(char, data_, (int)size_);
                }
            }

            char* data() const { return data_; }
            size_t size() const { return size_; }

            char* begin() const { return data_; }
            char* end() const { return data_ + size_; }

            void reset(bool release_memory = false)
            {
                if (release_memory && data_)
                {
                    DELETE_ARRAY_MP(char, data_, (int)size_);
                }
                data_ = nullptr;
                size_ = 0;
            }

        private:
            char* data_;
            size_t size_;
        };
    }
}