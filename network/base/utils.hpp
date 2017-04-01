#pragma once
#include <functional>
#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdint>
#include <boost/algorithm/string.hpp>

#define SWAP_LONG(l)                \
            ( ( ((l) >> 24) & 0x000000FFL ) |       \
              ( ((l) >>  8) & 0x0000FF00L ) |       \
              ( ((l) <<  8) & 0x00FF0000L ) |       \
              ( ((l) << 24) & 0xFF000000L ) )

#define SWAP_LONGLONG(l)            \
            ( ( ((l) >> 56) & 0x00000000000000FFLL ) |       \
              ( ((l) >> 40) & 0x000000000000FF00LL ) |       \
              ( ((l) >> 24) & 0x0000000000FF0000LL ) |       \
              ( ((l) >>  8) & 0x00000000FF000000LL ) |       \
              ( ((l) <<  8) & 0x000000FF00000000LL ) |       \
              ( ((l) << 24) & 0x0000FF0000000000LL ) |       \
              ( ((l) << 40) & 0x00FF000000000000LL ) |       \
              ( ((l) << 56) & 0xFF00000000000000LL ) )

namespace cytx {
    namespace rpc
    {
        inline bool retry(const std::function<bool()>& func, size_t max_attempts, size_t retry_interval = 0)
        {
            for (size_t i = 0; i < max_attempts; i++)
            {
                if (func())
                    return true;

                if (retry_interval > 0)
                    std::this_thread::sleep_for(std::chrono::milliseconds(retry_interval));
            }

            return false;
        }

        inline std::vector<boost::asio::ip::tcp::endpoint> get_tcp_endpoints(std::string const& address_port_string_list)
        {
            std::vector<std::string> address_port_list;
            boost::algorithm::split(address_port_list, address_port_string_list, boost::is_any_of(" ,|"));
            std::vector<boost::asio::ip::tcp::endpoint> tcp_endpoints;
            for (auto const& address_port : address_port_list)
            {
                auto pos = address_port.rfind(':');
                if (std::string::npos != pos)
                {
                    tcp_endpoints.emplace_back(
                        boost::asio::ip::address::from_string(address_port.substr(0, pos)),
                        boost::lexical_cast<uint16_t>(address_port.substr(pos + 1))
                    );
                }
            }

            return tcp_endpoints;
        }

        static boost::asio::ip::tcp::endpoint get_tcp_endpoint(std::string const& address, uint16_t port)
        {
            return{ boost::asio::ip::address::from_string(address), port };
        }

        static std::string domain2ip(const std::string& ip, uint16_t port)
        {
            boost::asio::io_service ios;
            boost::asio::ip::tcp::resolver slv(ios);
            boost::asio::ip::tcp::resolver::query qry(ip, boost::lexical_cast<std::string>(port));
            boost::asio::ip::tcp::resolver::iterator it = slv.resolve(qry);
            boost::asio::ip::tcp::resolver::iterator end;
            std::string host_ip;
            for (; it != end; it++)
            {
                if ((*it).endpoint().address().is_v6())
                {
                    return (*it).endpoint().address().to_string();
                }
                else if ((*it).endpoint().address().is_v4() && host_ip.empty())
                {
                    host_ip = (*it).endpoint().address().to_string();
                }
            }
            if (!host_ip.empty())
                return host_ip;
            return ip;
        }

        template<typename T>
        inline T ntoh(T t)
        {
            return t;
        }

        template<> inline short ntoh(short v)
        {
            return (short)ntohs(v);
        }

        template<> inline unsigned short ntoh(unsigned short v)
        {
            return ntohs(v);
        }

        template<> inline int ntoh(int v)
        {
            return (int)ntohl(v);
        }

        template<> inline unsigned int ntoh(unsigned int v)
        {
            return ntohl(v);
        }

        template<> inline int64_t ntoh(int64_t v)
        {
            return (int64_t)SWAP_LONGLONG((uint64_t)v);
        }

        template<> inline uint64_t ntoh(uint64_t v)
        {
            return (uint64_t)SWAP_LONGLONG(v);
        }

        template<> inline float ntoh(float v)
        {
            uint32_t tempval = *(uint32_t*)&v;
            auto ret = (uint32_t)SWAP_LONG(tempval);
            return *(float*)&ret;
        }

        template<> inline double ntoh(double v)
        {
            uint64_t tempval = *(uint64_t*)&v;
            auto ret = (uint64_t)SWAP_LONGLONG(tempval);
            return *(double*)&ret;
        }

        template<typename T>
        inline T hton(T t)
        {
            return t;
        }

        template<> inline short hton(short v)
        {
            return (short)htons(v);
        }

        template<> inline unsigned short hton(unsigned short v)
        {
            return htons(v);
        }

        template<> inline int hton(int v)
        {
            return (int)htonl(v);
        }

        template<> inline unsigned int hton(unsigned int v)
        {
            return htonl(v);
        }

        template<> inline int64_t hton(int64_t v)
        {
            return (int64_t)SWAP_LONGLONG((uint64_t)v);
        }

        template<> inline uint64_t hton(uint64_t v)
        {
            return (uint64_t)SWAP_LONGLONG(v);
        }

        template<> inline float hton(float v)
        {
            uint32_t tempval = *(uint32_t*)&v;
            auto ret = (uint32_t)SWAP_LONG(tempval);
            return *(float*)&ret;
        }

        template<> inline double hton(double v)
        {
            uint64_t tempval = *(uint64_t*)&v;
            auto ret = (uint64_t)SWAP_LONGLONG(tempval);
            return *(double*)&ret;
        }
    }
}
