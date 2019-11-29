#pragma once
#include "common.hpp"

namespace cytx
{
    namespace thrift
    {
        class thrift_transport : public TTransport
        {
        public:
            thrift_transport()
                : thrift_transport(nullptr)
            {}

            thrift_transport(connection_ptr conn_ptr)
                : conn_ptr_(conn_ptr)
            {}

            virtual ~thrift_transport()
            {
            }

            void set_conn_ptr(connection_ptr conn_ptr)
            {
                this->conn_ptr_ = conn_ptr;
            }

            uint32_t read_virt(uint8_t* buffer, uint32_t length) override
            {
                reader_.read_binary((char*)buffer, length);
                return length;
            }

            uint32_t readAll_virt(uint8_t* buf, uint32_t len) override
            {
                return read_virt(buf, len);
            }

            void write_virt(const uint8_t* buffer, uint32_t length) override
            {
                writer_.write_binary((const char*)const_cast<uint8_t*>(buffer), length);
            }

            const uint8_t* borrow_virt(uint8_t* buf, uint32_t* len) override
            {
                return (uint8_t*)reader_.data();
            }

            void consume_virt(uint32_t length) override
            {
                reader_.rd_ptr(length);
            }

            uint32_t readEnd() override
            {
                current_msg_ptr_ = nullptr;
                return 0;
            }

            uint32_t writeEnd() override
            {
                msg_ptr msg = std::make_shared<msg_t>(writer_);
                conn_ptr_->write(msg);

                writer_.reallocate(10240);
                return 0;
            }

            bool isOpen() override
            {
                return conn_ptr_->socket().is_open();
            }

            void close() override
            {
                conn_ptr_->close();
            }

            virtual void set_current_msg(msg_ptr msg)
            {
                current_msg_ptr_ = msg;
                reader_ = msg->get_stream();
            }
        protected:
            connection_ptr conn_ptr_;
            msg_ptr current_msg_ptr_;
            gos_t reader_{ 0 };
            gos_t writer_{ 10240 };
        };
    }
}