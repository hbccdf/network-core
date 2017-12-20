#pragma once
#include <string>
#include "MemoryPoolMgr.h"
#include <cstdint>

namespace cytx
{
    class GameObjectStream
    {
    public:
        static const int default_data_size = 1024;
    private:
        GameObjectStream(const GameObjectStream& stm) = delete;
        GameObjectStream& operator =(const GameObjectStream& stm) = delete;
        template<class T>
        T* pushValue(T t)
        {
            checkSpace(sizeof(T));
            char* pr = data_ + wr_pos_;
            memcpy(pr, &t, sizeof(T));
            wr_pos_ += sizeof(T);
            return (T*)pr;
        }

        template<class T>
        bool getValue(T& t)
        {
            if (int(rd_pos_ + sizeof(T)) > wr_pos_) return false;
            memcpy(&t, data_ + rd_pos_, sizeof(T));
            rd_pos_ += sizeof(T);
            return true;
        }
        template<class T>
        bool getBKValue(T& t)
        {
            if (wr_pos_ - (int)sizeof(T) < rd_pos_) return false;
            memcpy(&t, data_ + (wr_pos_ - sizeof(T)), sizeof(T));
            wr_pos_ -= sizeof(T);
            return true;
        }

    public:
        GameObjectStream() :wr_pos_(0), rd_pos_(0), data_size_(default_data_size), alloc_type_(1) { data_ = NEW_ARRAY_MP(char, data_size_); }
        GameObjectStream(int data_size) :wr_pos_(0), rd_pos_(0), data_size_(data_size), alloc_type_(1), data_(nullptr)
        {
            if (data_size_ > 0)
            {
                data_ = NEW_ARRAY_MP(char, data_size_);
            }
        }
        ///alloc_type 0:只读,不管理;1自动管理
        GameObjectStream(char* data, int len, int alloc_type = 0) :wr_pos_(len), rd_pos_(0), data_size_(len), alloc_type_(alloc_type)
        {
            if (alloc_type_ == 0)
            {
                data_ = data;
                return;
            }

            data_size_ = (len + 8) * 2;
            data_ = NEW_ARRAY_MP(char, data_size_);
            memcpy(data_, data, len);
        }
        GameObjectStream(GameObjectStream&& other)
            : wr_pos_(other.wr_pos_)
            , rd_pos_(other.rd_pos_)
            , data_size_(other.data_size_)
            , data_(other.data_)
            , alloc_type_(other.alloc_type_)
        {
            other.wr_pos_ = 0;
            other.rd_pos_ = 0;
            other.data_size_ = 0;
            other.data_ = nullptr;
            other.alloc_type_ = 0;
        }

        ~GameObjectStream() { if (alloc_type_ == 0) return; DELETE_ARRAY_MP(char, data_, data_size_); }
        void reset()
        {
            rd_pos_ = wr_pos_ = 0;
        }
        int crunch(void)
        {
            if (rd_pos_ != 0)
            {
                if (wr_pos_ <= rd_pos_)
                {
                    reset();
                    return 0;
                }
                //memcpy(data_, data_+rd_pos_, length());	//<memcpy虽然效率好一些但是会出现问题
                int len = length();
                memmove(data_, data_ + rd_pos_, len);
                rd_pos_ = 0;
                wr_pos_ = len;
            }
            return 0;
        }
        void checkSpace(int len)
        {
            if (data_size_ >= wr_pos_ + len)
                return;
            int data_size = (wr_pos_ + len) * 2;
            char* p = NEW_ARRAY_MP(char, data_size);
            memcpy(p, data_, wr_pos_);
            DELETE_ARRAY_MP(char, data_, data_size_);
            data_size_ = data_size;
            data_ = p;
        }
        void reallocate_data(int len)
        {
            if (len > 0)
            {
                data_ = NEW_ARRAY_MP(char, len);

                wr_pos_ = 0;
                rd_pos_ = 0;
                data_size_ = len;
                alloc_type_ = 1;
            }
        }
        int length() const { return wr_pos_ > rd_pos_ ? wr_pos_ - rd_pos_ : 0; }
        int space() const { return data_size_ - wr_pos_; }
        int rd_pos() const { return rd_pos_; }
        int wr_pos() const { return wr_pos_; }
        char* rd_ptr() { return rd_pos_ <= wr_pos_ ? (data_ + rd_pos_) : NULL; }
        char* rd_ptr(int offset) { offset += rd_pos_; if (offset<0 || offset>wr_pos_)return NULL; rd_pos_ = offset; return data_ + rd_pos_; }
        void rd_ptr(char* ptr) { int new_pos = int(ptr - data_); rd_pos_ = new_pos; }
        char* wr_ptr() { return wr_pos_ < data_size_ ? (data_ + wr_pos_) : NULL; }
        char* wr_ptr(int offset) { offset += wr_pos_; if (offset<0 || offset>data_size_)return NULL; wr_pos_ = offset; return data_ + wr_pos_; }
        void pushBool(bool val)
        {
            unsigned char uVal = val ? 1 : 0;
            pushUChar(uVal);
        }
        void pushChar(char val) { pushValue(val); }
        unsigned char* pushUChar(unsigned char val) { return pushValue(val); }
        short* pushShort(short val) { return pushValue(val); }
        unsigned short* pushUShort(unsigned short val) { return pushValue(val); }
        int* pushInt(int val) { return pushValue(val); }
        unsigned int* pushUInt(unsigned int val) { return pushValue(val); }
        void pushUint32(uint32_t val) { pushValue(val); }
        void pushFloat(float val) { pushValue(val); }
        void pushDouble(double val) { pushValue(val); }
        void pushInt64(int64_t val) { pushValue(val); }
        void pushVoid(void* val) { pushValue(val); }
        void pushString(const std::string& str)
        {
            int len = int(str.length());
            checkSpace(len + sizeof(short));
            memcpy(data_ + wr_pos_, &len, sizeof(short));
            wr_pos_ += sizeof(short);
            memcpy(data_ + wr_pos_, str.c_str(), len);
            wr_pos_ += len;
        }
        void pushWstring(const std::wstring& wStr)
        {
            int len = int(wStr.length());
            checkSpace(len * sizeof(wchar_t) + sizeof(short));
            memcpy(data_ + wr_pos_, &len, sizeof(short));
            wr_pos_ += sizeof(short);
            memcpy(data_ + wr_pos_, (char*)wStr.c_str(), len * sizeof(wchar_t));
            wr_pos_ += len * sizeof(wchar_t);
        }
        void pushObject(void* ptr, short object_size)
        {
            checkSpace(object_size + sizeof(short));
            memcpy(data_ + wr_pos_, &object_size, sizeof(short));
            wr_pos_ += sizeof(short);
            memcpy(data_ + wr_pos_, ptr, object_size);
            wr_pos_ += object_size;
        }
        void pushBinary(const char* ptr, short len)
        {
            checkSpace(len);
            memcpy(data_ + wr_pos_, ptr, len);
            wr_pos_ += len;
        }
        void pushPair(int key, double dval)
        {
            pushInt(key);
            pushDouble(dval);
        }
        bool setPairValue(int key, double dval)
        {
            //unsigned int psize = sizeof(int)+1+sizeof(double)+1;
            int rd_index = 0;
            while (rd_index < wr_pos_)
            {
                int val = 0;
                memcpy(&val, data_ + rd_index, sizeof(int));
                rd_index += sizeof(int);
                if (val == key)
                {
                    memcpy(data_ + rd_index, &dval, sizeof(double));
                    return true;
                }
                rd_index += sizeof(double);
            }
            pushPair(key, dval);
            return true;
        }
        bool getPairIter(int& key, double& dval)
        {
            int rd_index = rd_pos_;
            memcpy(&key, data_ + rd_index, sizeof(int));
            rd_index += sizeof(int);
            memcpy(&dval, data_ + rd_index, sizeof(double));
            rd_index += sizeof(double);
            rd_pos_ = rd_index;
            return true;
        }
        bool getPairValue(int key, double& dval)
        {
            //unsigned int psize = sizeof(int)+1+sizeof(double)+1;
            int rd_index = 0;
            while (rd_index < wr_pos_)
            {
                int val = 0;
                memcpy(&val, data_ + rd_index, sizeof(int));
                rd_index += sizeof(int);
                if (val == key)
                {
                    memcpy(&dval, data_ + rd_index, sizeof(double));
                    return true;
                }
                rd_index += sizeof(double);
            }
            return false;
        }
        bool getBool(bool& val)
        {
            unsigned char uVal = 0;
            bool bRet = getUChar(uVal);
            val = (uVal == 1);

            return bRet;
        }
        bool getChar(char& val) { return getValue(val); }
        bool getUChar(unsigned char& val) { return getValue(val); }
        bool getShort(short& val) { return getValue(val); }
        bool getUShort(unsigned short& val) { return getValue(val); }
        bool getInt(int& val) { return getValue(val); }
        bool getUInt(unsigned int& val) { return getValue(val); }
        bool getFloat(float& val) { return getValue(val); }
        bool getDouble(double& val) { return getValue(val); }
        bool getInt64(int64_t& val) { return getValue(val); }
        bool getVoid(void*& val) { return getValue(val); }
        bool getString(std::string& str)
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
        int getObject(char* obj, int obj_size)
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
        bool getBinary(char* rd_buff, int rd_sz)
        {
            memcpy(rd_buff, data_ + rd_pos_, rd_sz);
            rd_pos_ += rd_sz;
            return true;
        }
        bool getWstring(std::wstring& wStr)
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
        }
        bool getBKUint64(uint64_t& val)
        {
            return getBKValue(val);
        }
        bool getBKUint32(uint32_t& val)
        {
            return getBKValue(val);
        }

    public:
        char*       data_;
        int         alloc_type_;//0:只读,不管理;1自动管理
        int         wr_pos_;
        int         rd_pos_;
        int         data_size_;
    };
}
