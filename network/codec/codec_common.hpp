#pragma once
#include "../base/GameObjectStream.h"
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
            {
                size_ = end - begin;
                data_ = NEW_ARRAY_MP(char, (int)size_);
                memcpy(data_, begin, size_);
            }
            ~gos_buffer()
            {
                if (data_)
                    DELETE_ARRAY_MP(char, data_, (int)size_);
            }

            char* data_;
            size_t size_;

            char* data() { return data_; }
            size_t size() { return size_; }

            char* begin() { return data_; }
            char* end() { return data_ + size_; }
        };
    }
}