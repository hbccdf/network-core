#pragma once
#include <string>
#include <cstdint>
#include "memory_pool.hpp"

namespace cytx
{
    class memory_stream
    {
    public:
        memory_stream()
            : memory_stream(_default_data_size)
        {
        }
        memory_stream(int data_size)
            : data_(nullptr)
            , data_size_(data_size)
            , alloc_type_(1)
            , wr_pos_(0)
            , rd_pos_(0)
        {
            if (data_size_ > 0)
            {
                data_ = MALLOC_MP(char, data_size_);
            }
        }
        ///alloc_type 0:只读,不管理;1自动管理
        memory_stream(char* data, int len, int alloc_type = 0)
            : wr_pos_(len)
            , rd_pos_(0)
            , data_size_(len)
            , alloc_type_(alloc_type)
        {
            if (alloc_type_ == 0)
            {
                data_ = data;
                return;
            }

            data_size_ = (len + 8) * 2;
            data_ = MALLOC_MP(char, data_size_);
            memcpy(data_, data, len);
        }
        memory_stream(memory_stream&& other)
            : data_(other.data_)
            , data_size_(other.data_size_)
            , alloc_type_(other.alloc_type_)
            , wr_pos_(other.wr_pos_)
            , rd_pos_(other.rd_pos_)
        {
            other.data_ = nullptr;
            other.data_size_ = 0;
            other.alloc_type_ = 0;
            other.wr_pos_ = 0;
            other.rd_pos_ = 0;
        }

        ~memory_stream()
        {
            if (alloc_type_ != 0 && data_ != nullptr)
            {
                FREE_MP(data_);
            }
        }

        memory_stream& operator =(const memory_stream&& other)
        {
            if (alloc_type_ != 0 && data_ != nullptr)
            {
                FREE_MP(data_);
            }
            data_ = other.data_;
            data_size_ = other.data_size_;
            alloc_type_ = other.alloc_type_;
            wr_pos_ = other.wr_pos_;
            rd_pos_ = other.rd_pos_;
        }

        void reset()
        {
            rd_pos_ = wr_pos_ = 0;
        }
        int crunch()
        {
            if (rd_pos_ != 0)
            {
                if (wr_pos_ <= rd_pos_)
                {
                    reset();
                    return 0;
                }
                //<memcpy虽然效率好一些但是会出现问题
                //memcpy(data_, data_+rd_pos_, length());
                int len = length();
                memmove(data_, data_ + rd_pos_, len);
                rd_pos_ = 0;
                wr_pos_ = len;
            }
            return 0;
        }
        void check_space(int len)
        {
            if (data_size_ >= wr_pos_ + len)
                return;
            int data_size = (wr_pos_ + len) * 2;
            char* p = MALLOC_MP(char, data_size);
            memcpy(p, data_, wr_pos_);
            FREE_MP(data_);
            data_size_ = data_size;
            data_ = p;
        }
        void reallocate_data(int len)
        {
            if (len > 0)
            {
                data_ = MALLOC_MP(char, len);

                wr_pos_ = 0;
                rd_pos_ = 0;
                data_size_ = len;
                alloc_type_ = 1;
            }
        }
        int length() const
        {
            return wr_pos_ > rd_pos_ ? wr_pos_ - rd_pos_ : 0;
        }
        int space() const
        {
            return data_size_ - wr_pos_;
        }
        int rd_pos() const { return rd_pos_; }
        int wr_pos() const { return wr_pos_; }
        char* rd_ptr() const
        {
            return rd_pos_ <= wr_pos_ ? (data_ + rd_pos_) : nullptr;
        }
        char* rd_ptr(int offset)
        {
            offset += rd_pos_;
            if (offset < 0 || offset > wr_pos_)
                return nullptr;

            rd_pos_ = offset;
            return data_ + rd_pos_;
        }
        void rd_ptr(char* ptr)
        {
            int new_pos = int(ptr - data_);
            rd_pos_ = new_pos;
        }
        char* wr_ptr() const
        {
            return wr_pos_ < data_size_ ? (data_ + wr_pos_) : nullptr;
        }
        char* wr_ptr(int offset)
        {
            offset += wr_pos_;
            if (offset<0 || offset>data_size_)
                return nullptr;

            wr_pos_ = offset;
            return data_ + wr_pos_;
        }

    public:
        template<class T>
        void write_value(T t)
        {
            check_space(sizeof(T));
            char* pr = data_ + wr_pos_;
            memcpy(pr, &t, sizeof(T));
            wr_pos_ += sizeof(T);
        }

        template<class T>
        bool read_value(T& t)
        {
            if (int(rd_pos_ + sizeof(T)) > wr_pos_)
                return false;
            memcpy(&t, data_ + rd_pos_, sizeof(T));
            rd_pos_ += sizeof(T);
            return true;
        }

        void write_bool(bool val)
        {
            uint8_t uval = val ? 1 : 0;
            write_uchar(uval);
        }
        void write_char(char val)
        {
            write_value(val);
        }
        void write_uchar(unsigned char val)
        {
            write_value(val);
        }
        void write_short(short val)
        {
            write_value(val);
        }
        void write_ushort(unsigned short val)
        {
            write_value(val);
        }
        void write_int(int val)
        {
            write_value(val);
        }
        void write_uint(uint32_t val)
        {
            write_value(val);
        }
        void write_float(float val)
        {
            write_value(val);
        }
        void write_double(double val)
        {
            write_value(val);
        }
        void write_int64(int64_t val)
        {
            write_value(val);
        }
        void write_uint64(uint64_t val)
        {
            write_value(val);
        }

        void write_string(const std::string& str)
        {
            int len = int(str.length());
            size_t length_size = sizeof(short);
            check_space(len + (int)length_size);
            memcpy(data_ + wr_pos_, &len, length_size);
            wr_pos_ += (int)length_size;
            memcpy(data_ + wr_pos_, str.c_str(), len);
            wr_pos_ += len;
        }
        void write_wstring(const std::wstring& wstr)
        {
            int len = int(wstr.length());
            check_space(len * sizeof(wchar_t) + sizeof(short));
            memcpy(data_ + wr_pos_, &len, sizeof(short));
            wr_pos_ += sizeof(short);
            memcpy(data_ + wr_pos_, (char*)wstr.c_str(), len * sizeof(wchar_t));
            wr_pos_ += len * sizeof(wchar_t);
        }
        void write_object(void* ptr, short object_size)
        {
            check_space(object_size + sizeof(short));
            memcpy(data_ + wr_pos_, &object_size, sizeof(short));
            wr_pos_ += sizeof(short);
            memcpy(data_ + wr_pos_, ptr, object_size);
            wr_pos_ += object_size;
        }
        void write_binary(const char* ptr, int len)
        {
            check_space(len);
            memcpy(data_ + wr_pos_, ptr, len);
            wr_pos_ += len;
        }

        bool read_bool(bool& val)
        {
            unsigned char uval = 0;
            bool ret = read_uchar(uval);
            val = (uval == 1);

            return ret;
        }
        bool read_char(char& val)
        {
            return read_value(val);
        }
        bool read_uchar(unsigned char& val)
        {
            return read_value(val);
        }
        bool read_short(short& val)
        {
            return read_value(val);
        }
        bool read_ushort(unsigned short& val)
        {
            return read_value(val);
        }
        bool read_int(int& val)
        {
            return read_value(val);
        }
        bool read_uint(unsigned int& val)
        {
            return read_value(val);
        }
        bool read_int64(int64_t& val)
        {
            return read_value(val);
        }
        bool read_uint64(uint64_t& val)
        {
            return read_value(val);
        }
        bool read_float(float& val)
        {
            return read_value(val);
        }
        bool read_double(double& val)
        {
            return read_value(val);
        }

        bool read_string(std::string& str)
        {
            if (wr_pos_ <= int(rd_pos_ + sizeof(short)))
                return false;
            short len = 0;
            memcpy(&len, data_ + rd_pos_, sizeof(short));
            rd_pos_ += sizeof(short);
            if (len == 0)
            {
                str = "";
                return true;
            }
            char c = *(data_ + rd_pos_ + len);
            *(data_ + rd_pos_ + len) = '\0';
            str = data_ + rd_pos_;
            *(data_ + rd_pos_ + len) = c;
            rd_pos_ += len;
            return true;
        }
        bool read_wstring(std::wstring& wStr)
        {
            if (wr_pos_ <= int(rd_pos_ + sizeof(short)))
                return false;

            short len = 0;
            memcpy(&len, data_ + rd_pos_, sizeof(short));
            rd_pos_ += sizeof(short);

            if (len == 0)
            {
                wStr = L"";
                return true;
            }

            wchar_t *p = (wchar_t *)(data_ + rd_pos_);
            wchar_t c = *(p + len);
            *(p + len) = L'\0';

            wStr = p;
            *(p + len) = c;

            rd_pos_ += len * sizeof(wchar_t);
            return true;
        }
        int read_object(char* obj, int obj_size)
        {
            short len = 0;
            memcpy(&len, data_ + rd_pos_, sizeof(short));
            rd_pos_ += sizeof(short);
            if (len > obj_size)
            {
                rd_pos_ -= sizeof(short) + 1;
                return len;
            }
            memcpy(obj, data_ + rd_pos_, len);
            rd_pos_ += len;
            return len;
        }
        bool read_binary(char* rd_buff, int rd_sz)
        {
            memcpy(rd_buff, data_ + rd_pos_, rd_sz);
            rd_pos_ += rd_sz;
            return true;
        }

    public:
        void set_alloc_type(int alloc_type)
        {
            alloc_type_ = alloc_type;
        }

        char* data() const { return rd_ptr(); }

        char* raw_data() const { return data_; }

        int raw_length() const { return data_size_; }

    public:
        static int defualt_size()
        {
            return _default_data_size;
        }
    private:
        memory_stream(const memory_stream& stm) = delete;
        memory_stream& operator =(const memory_stream& stm) = delete;

    private:
        char*       data_;
        int         data_size_;
        int         alloc_type_;//0:只读,不管理;1自动管理
        int         wr_pos_;
        int         rd_pos_;

    private:
        constexpr static int _default_data_size = 1024;
    };
}
